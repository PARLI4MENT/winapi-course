#include <Windows.h>

#include <iostream>

#include "HeapManager.h"
#include "Exception.h"

CBlock::CBlock( int _size, void* _ptr ) :
	size( _size ),
	ptr( _ptr )
{
}

int CBlock::GetSize() const
{
	return size;
}

void* CBlock::GetPtr() const
{
	return ptr;
}

//----------------------------------------------------------------------------------------------------------------------

bool CBlockComparatorBySize::operator()( const CBlock & left, const CBlock & right ) const
{
	return left.GetSize() < right.GetSize() || left.GetSize() == right.GetSize() && left.GetPtr() < right.GetPtr();
}

//----------------------------------------------------------------------------------------------------------------------

bool CBlockComparatorByPtr::operator()( const CBlock & left, const CBlock & right ) const
{
	return left.GetPtr() < right.GetPtr() || left.GetPtr() == right.GetPtr() && left.GetSize() < right.GetSize();
}

//----------------------------------------------------------------------------------------------------------------------

CHeapManager::CHeapManager()
{
}

CHeapManager::~CHeapManager()
{
}

void CHeapManager::Create( int minSize, int maxSize )
{
	if ( minSize < 0 || minSize > maxSize || maxSize < 0 ) {
		throw CIncorrectSize( "CHeapManager::Create: Either size is negative or minimum size greater than maximum size." );
	}

	memorySize = roundUpToNBytes( maxSize, pageSize );

	reserveMemory();
	commitMemory( roundUpToNBytes( minSize, pageSize ) );

	initializePages();
	initializeSets();
}

void* CHeapManager::Alloc( int size )
{
	if ( size < 0 ) {
		throw CIncorrectSize( "CHeapManager::Alloc: Size is negative." );;
	}

	int actualSize = roundUpToNBytes( size + sizeof( int ), minBlockSize );
	CBlock freeBlock = takeFreeBlock( actualSize );
	void* ptr = freeBlock.GetPtr();

	allocatedBlocks.insert( CBlock( actualSize, ptr ) );
	upPages( ptr, actualSize );
	commitBlocks( ptr, actualSize );

	*static_cast<int*>( ptr ) = actualSize; // Write block size to the first bytes.
	return offsetPtr( ptr, sizeof( int ) ); // Return ptr considered writing above.
}

void CHeapManager::Free( void* mem )
{
	void* ptr = offsetPtr( mem, -static_cast<int>( sizeof( int ) ) );
	int size = *static_cast<int*>( ptr );

	allocatedBlocks.erase( CBlock( size, ptr ) );
	downPages( ptr, size );
	insertBlock( CBlock( size, ptr ) );
}

void CHeapManager::Destroy()
{
	if ( VirtualFree( reservedMemoryPtr, 0, MEM_RESET ) != NULL ) {
		throw CVirtualAllocError( "CHeapManager::Destroy: Cannot reset memory." );
	}

	for ( auto block : allocatedBlocks ) {
		std::cerr << block.GetPtr() << " " << block.GetSize() << std::endl;
	}
}

int CHeapManager::roundUpToNBytes( int bytes, int multiplicity ) const
{
	if ( bytes == 0 ) {
		return multiplicity;
	}
	if ( bytes % multiplicity == 0 ) {
		return bytes;
	}
	return bytes - ( bytes % multiplicity ) + multiplicity;
}

void CHeapManager::initializePages()
{
	pages.assign( memorySize / pageSize, 0 );
}

void CHeapManager::initializeSets()
{
	insertBlock( CBlock( memorySize, reservedMemoryPtr ) );
}

void CHeapManager::reserveMemory()
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366887%28v=vs.85%29.aspx.
	reservedMemoryPtr = VirtualAlloc( nullptr, memorySize, MEM_RESERVE, PAGE_READWRITE );
	if ( reservedMemoryPtr == nullptr ) {
		throw CVirtualAllocError( "CHeapManager::reserveMemory: Cannot reserve memory." );
	}
}

void CHeapManager::commitMemory( int size ) const
{
	LPVOID commitedMemoryPtr = VirtualAlloc( reservedMemoryPtr, size, MEM_COMMIT, PAGE_READWRITE );
	if ( commitedMemoryPtr == nullptr ) {
		throw CVirtualAllocError( "CHeapManager::commitMemory: Cannot commit memory." );
	} else if ( commitedMemoryPtr != reservedMemoryPtr ) {
		throw CVirtualAllocError( "CHeapManager::commitMemory: The postd::size_ters to commited and reserved memory are different." );
	}
}

CBlock CHeapManager::takeFreeBlock( int size )
{
	CBlock block = getFreeBlock( size );
	CBlock residualBlock = getResidualBlock( block, size );
	updateSets( block, residualBlock );
	return CBlock( size, block.GetPtr() );
}

CBlock CHeapManager::getFreeBlock( int size ) const
{
	auto freeBlockIterator = freeBlocksSetsComparedBySize.lower_bound( CBlock( size, nullptr ) );
	if ( freeBlockIterator == freeBlocksSetsComparedBySize.end() ) {
		throw CMemoryLimitExceeded( "CHeapManager::getFreeBlock: Memory limit exceeded." );
	}
	return *freeBlockIterator;
}

CBlock CHeapManager::getResidualBlock( CBlock block, int size ) const
{
	int residualSize = block.GetSize() - size;
	void* ptr = offsetPtr( block.GetPtr(), size );
	return CBlock( residualSize, ptr );
}

int CHeapManager::getCommitingSize( int size, void* ptr ) const
{
	int pageOffset = getPageOffset( ptr );
	if ( pageOffset + ( size % pageSize ) > pageSize ) {
		// Example. We have two pages:  ffff ffff (f - free)
		// Alloc( 3 * 1024 ):           aaaf ffff (a - allocated)
		// Alloc (2 * 1024 ):           aaaa afff
		// But VirtualAlloc rounds down ptr to 4 Kb, so without the next addition we won't commit the second page.
		size += pageSize;
	}
	return size;
}

int CHeapManager::getPageOffset( void* ptr ) const
{
	return  getIntByPtr( ptr ) % pageSize;
}

int CHeapManager::getIntByPtr( void* ptr ) const
{
	return reinterpret_cast<std::uintptr_t>( ptr );
}

void CHeapManager::upPages( void* ptr, int size )
{
	void* leftPtr = ptr;
	void* rightPtr = offsetPtr( ptr, size - minBlockSize );
	int leftPageIndex = getPageIndex( leftPtr );
	int rightPageIndex = getPageIndex( rightPtr );
	int leftPageOffset = getPageOffset( leftPtr );
	int rightPageOffset = getPageOffset( rightPtr );

	if ( leftPageIndex == rightPageIndex ) {
		pages[leftPageIndex] += rightPageOffset - leftPageOffset + minBlockSize;
		return;
	}

	pages[leftPageIndex] += pageSize - leftPageOffset;
	for ( int i = leftPageIndex + 1; i <= rightPageIndex - 1; ++i ) {
		pages[i] = pageSize;
	}
	pages[rightPageIndex] += rightPageOffset + minBlockSize;
}

int  CHeapManager::getPageIndex( void* ptr ) const
{
	int current = getIntByPtr( ptr ) / pageSize;
	int start = getIntByPtr( reservedMemoryPtr ) / pageSize;
	return current - start;
}

void* CHeapManager::offsetPtr( void* ptr, int offset ) const
{
	return static_cast<void*>( static_cast<char*>( ptr ) + offset );
}

void CHeapManager::downPages( void* ptr, int size )
{
	void* leftPtr = ptr;
	void* rightPtr = offsetPtr( ptr, size - minBlockSize );
	int leftPageIndex = getPageIndex( leftPtr );
	int rightPageIndex = getPageIndex( rightPtr );
	int leftPageOffset = getPageOffset( leftPtr );
	int rightPageOffset = getPageOffset( rightPtr );

	if ( leftPageIndex == rightPageIndex ) {
		pages[leftPageIndex] -= rightPageOffset - leftPageOffset + minBlockSize;
		if ( pages[leftPageIndex] == 0 ) {
			void* freeingPtr = offsetPtr( ptr, -leftPageOffset );
			// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366892(v=vs.85).aspx.
			if ( VirtualFree( freeingPtr, pageSize, MEM_DECOMMIT ) != TRUE ) {
				throw CVirtualFreeError( "CHeapManager::downPages: Cannot free memory." );
			}
		}
		return;
	}

	pages[leftPageIndex] -= pageSize - leftPageOffset;
	for ( int i = leftPageIndex + 1; i <= rightPageIndex - 1; ++i ) {
		pages[i] = 0;
	}
	pages[rightPageIndex] -= rightPageOffset + minBlockSize;

	void* freeingPtr = offsetPtr( ptr, -leftPageOffset );
	int freeingSize = size + leftPageOffset + ( pageSize - rightPageOffset - minBlockSize );

	if ( pages[leftPageIndex] != 0 ) {
		freeingPtr = offsetPtr( freeingPtr, pageSize );
		freeingSize -= pageSize;
	}
	if ( pages[rightPageIndex] != 0 ) {
		freeingSize -= pageSize;
	}

	if ( freeingSize != 0 ) {
		if ( VirtualFree( freeingPtr, freeingSize, MEM_DECOMMIT ) != TRUE ) {
			throw CVirtualFreeError( "CHeapManager::downPages: Cannot free memory." );
		}
	}
}

void CHeapManager::commitBlocks( void* ptr, int size )
{
	void* leftPtr = ptr;
	void* rightPtr = offsetPtr( ptr, size - minBlockSize );
	int leftPageOffset = getPageOffset( leftPtr );
	int rightPageOffset = getPageOffset( rightPtr );

	void* commitingPtr = offsetPtr( ptr, -leftPageOffset );
	int commitingSize = size + leftPageOffset + ( pageSize - rightPageOffset - minBlockSize );

	if ( VirtualAlloc( commitingPtr, commitingSize, MEM_COMMIT, PAGE_READWRITE ) == nullptr ) {
		throw CVirtualAllocError( "CHeapManager::commitBlocks: Cannot commit memory." );
	}
}

void CHeapManager::updateSets( CBlock oldBlock, CBlock newBlock )
{
	eraseBlock( oldBlock );
	insertBlock( newBlock );
}

void CHeapManager::eraseBlock( CBlock block )
{
	freeBlocksSetsComparedByPtr.erase( block );
	freeBlocksSetsComparedBySize.erase( block );
}

void CHeapManager::insertBlock( CBlock block )
{
	auto rightIterator = freeBlocksSetsComparedByPtr.upper_bound( block );
	if ( rightIterator != freeBlocksSetsComparedByPtr.end() && !freeBlocksSetsComparedByPtr.empty() ) {
		CBlock right = *rightIterator;
		if ( right.GetPtr() == offsetPtr( block.GetPtr(), block.GetSize() ) ) {
			// It is block's neighbour.
			eraseBlock( right );
			insertBlock( CBlock( block.GetSize() + right.GetSize(), block.GetPtr() ) );
			return;
		}
	}

	auto leftIterator = freeBlocksSetsComparedByPtr.upper_bound( block );
	if ( leftIterator != freeBlocksSetsComparedByPtr.begin() && !freeBlocksSetsComparedByPtr.empty() ) {
		--leftIterator;
		CBlock left = *leftIterator;
		if ( block.GetPtr() == offsetPtr( left.GetPtr(), left.GetSize() ) ) {
			// It is block's neighbour.
			eraseBlock( left );
			insertBlock( CBlock( left.GetSize() + block.GetSize(), left.GetPtr() ) );
			return;
		}
	}

	freeBlocksSetsComparedByPtr.insert( block );
	freeBlocksSetsComparedBySize.insert( block );
}

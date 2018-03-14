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

	memorySize = roundUpToNBytes( 2 * maxSize, pageSize );

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

	int commitingSize = getCommitingSize( actualSize, ptr );
	if ( VirtualAlloc( ptr, commitingSize, MEM_COMMIT, PAGE_READWRITE ) == nullptr ) {
		throw CVirtualAllocError( "CHeapManager::Alloc: Cannot commit memory." );
	}

	allocatedBlocks.insert( CBlock( actualSize, ptr ) );
	upPages( ptr, actualSize );

	*static_cast<int*>( ptr ) = actualSize; // Write block size to the first bytes.
	return offsetPtr( ptr, sizeof( int ) ); // Retunr ptr considered writing above.
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
	if ( VirtualAlloc( reservedMemoryPtr, memorySize, MEM_RESET, PAGE_NOACCESS ) != NULL ) {
		throw CVirtualAllocError( "CHeapManager::Destroy: Cannot reset memory." );
	}

	for ( auto block : allocatedBlocks ) {
		std::cerr << block.GetPtr() << " " << block.GetSize() << std::endl;
	}
}

int CHeapManager::roundUpToNBytes( int bytes, int multiplicity ) const
{
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

void CHeapManager::reserveMemory( )
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366887%28v=vs.85%29.aspx.
	reservedMemoryPtr = VirtualAlloc( nullptr, memorySize, MEM_RESERVE, PAGE_READWRITE );
	if ( reservedMemoryPtr == nullptr ) {
		throw CVirtualAllocError( "CHeapManager::reserveMemory: Cannot reserve memory.");
	}
}

void CHeapManager::commitMemory( int size ) const
{
	LPVOID commitedMemoryPtr = VirtualAlloc( reservedMemoryPtr, size, MEM_COMMIT, PAGE_READWRITE );
	if ( commitedMemoryPtr == nullptr ) {
		throw CVirtualAllocError( "CHeapManager::commitMemory: Cannot commit memory." );
	} else if ( commitedMemoryPtr != reservedMemoryPtr ) {
		throw CVirtualAllocError( "CHeapManager::commitMemory: The pointers to commited and reserved memory are different." );
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

int  CHeapManager::getIntByPtr( void* ptr ) const
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
		pages[leftPageIndex] += rightPageOffset - leftPageOffset + 1;
		return;
	}

	pages[leftPageIndex] += maxBlocksPerPage - leftPageOffset + 1;
	for ( int i = leftPageIndex + 1; i <= rightPageOffset - 1; ++i ) {
		pages[i] = maxBlocksPerPage;
	}
	pages[leftPageIndex] += rightPageOffset;
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
		pages[leftPageIndex] -= rightPageOffset - leftPageOffset + 1;

		if ( pages[leftPageIndex] == 0 ) {
			// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366892(v=vs.85).aspx.
			if ( VirtualFree( leftPtr, pageSize, MEM_DECOMMIT ) != TRUE ) {
				CVirtualFreeError( "CHeapManager::downPages: Cannot free memory." );
			}
		}
		return;
	}

	pages[leftPageIndex] -= maxBlocksPerPage - leftPageOffset + 1;
	for ( int i = leftPageIndex + 1; i <= rightPageOffset - 1; ++i ) {
		pages[i] = 0;
	}
	pages[leftPageIndex] -= rightPageOffset;

	// Use rounding magic for simple calculations.
	void* freeingPtr = ptr;
	int freeingSize = size;

	if ( pages[leftPageIndex] != 0 ) {
		freeingPtr = offsetPtr( freeingPtr, pageSize );
	}
	if ( pages[leftPageIndex] != 0 ) {
		freeingSize -= pageSize;
	}

	if ( VirtualFree( freeingPtr, freeingSize, MEM_DECOMMIT ) != TRUE ) {
		throw CVirtualFreeError("CHeapManager::downPages: Cannot free memory.");
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

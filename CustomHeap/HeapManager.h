#pragma once

#include <Windows.h>

#include <array>
#include <set>
#include <vector>

class CBlock {

public:
	CBlock( int _size, void* _ptr );

	int GetSize() const;
	void* GetPtr() const;

private:
	int size;
	void* ptr;
};

//----------------------------------------------------------------------------------------------------------------------

struct CBlockComparatorBySize {
	bool operator()( const CBlock& left, const CBlock& right ) const;
};

//----------------------------------------------------------------------------------------------------------------------

struct CBlockComparatorByPtr {
	bool operator()( const CBlock& left, const CBlock& right ) const;
};

//----------------------------------------------------------------------------------------------------------------------

class CHeapManager {

public:
	CHeapManager();
	~CHeapManager();

	void Create( int minSize, int maxSize );
	void* Alloc( int size );
	void Free( void* mem );
	void Destroy();

private:
	static const int pageSize = 4096;
	static const int minBlockSize = 4;
	static const int maxBlocksPerPage = pageSize / minBlockSize;
	LPVOID reservedMemoryPtr;
	int memorySize;
	std::vector<int> pages;
	std::set<CBlock, CBlockComparatorBySize> freeBlocksSetsComparedBySize;
	std::set<CBlock, CBlockComparatorByPtr> freeBlocksSetsComparedByPtr;
	std::set<CBlock, CBlockComparatorByPtr> allocatedBlocks;

	int roundUpToNBytes( int bytes, int multiplicity ) const;
	void initializePages();
	void initializeSets();
	void reserveMemory();
	void commitMemory( int size ) const;
	CBlock takeFreeBlock( int size );
	CBlock getFreeBlock( int size ) const;
	CBlock getResidualBlock( CBlock block, int size )  const;
	int getCommitingSize( int size, void* ptr ) const;
	int getPageOffset( void* ptr ) const;
	int getIntByPtr( void* ptr ) const;
	void commitBlocks( void* ptr, int size );
	void upPages( void* ptr, int size );
	int getPageIndex( void* ptr ) const;
	void* offsetPtr( void* ptr, int offset ) const;
	void downPages( void* ptr, int size );
	void updateSets( CBlock oldBlock, CBlock newBlock );
	void eraseBlock( CBlock block );
	void insertBlock( CBlock block );
};

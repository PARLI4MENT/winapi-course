#include "Class.h"
#include "HeapManager.h"

#include <iostream>

CHeapManager heapManager;

void initHeapManager( int size )
{
	heapManager.Create( 0, size );
}

void clearHeapManager()
{
	heapManager.Destroy();
}

//----------------------------------------------------------------------------------------------------------------------

void* CClass::operator new( std::size_t size )
{
	return heapManager.Alloc( size );
}

void CClass::operator delete( void * mem )
{
	heapManager.Free( mem );
}

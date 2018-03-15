#pragma once

#include "HeapManager.h"

void initHeapManager( int size );
void clearHeapManager();

//----------------------------------------------------------------------------------------------------------------------

class CClass {

public:
	void* operator new( std::size_t size );
	void operator delete( void* mem );
};

//----------------------------------------------------------------------------------------------------------------------

class CClassXS : public CClass {

private:
	char data[1];
};

//----------------------------------------------------------------------------------------------------------------------

class CClassS : public CClass {

private:
	CClassXS data[2];
};

//----------------------------------------------------------------------------------------------------------------------

class CClassM : public CClass {

private:
	CClassS data[4];
};

//----------------------------------------------------------------------------------------------------------------------

class CClassL : public CClass {

private:
	CClassM data[8];
};

//----------------------------------------------------------------------------------------------------------------------

class CClassXL : public CClass {

private:
	CClassL data[16];
};

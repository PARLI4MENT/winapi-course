#include <Windows.h>

#include <chrono> 
#include <iostream>
#include <random>

#include "HeapManager.h"
#include "Class.h"
#include "Struct.h"

const int MinClass = 0;
const int MaxClass = 4;
const int SampleCount = 100;
const int OperationCount = 10000;

std::vector<int> types;
std::vector<std::pair<int, int>> operations;

CClass* classes[SampleCount];
CStruct* structs[SampleCount];

template<class CBase, class CDerived>
void doOperation( CBase* data[], int operation, int index )
{
	if ( operation >= 1 && data[index] == nullptr ) {
		data[index] = new CDerived;
	} else if ( operation == 0 && data[index] != nullptr ) {
		delete data[index];
		data[index] = nullptr;
	}
}

template<class CBase, class CDerived0, class CDerived1, class CDerived2, class CDerived3, class CDerived4>
void test( CBase* data[] )
{
	for ( int i = 0; i < OperationCount; ++i ) {
		int index = operations[i].first;
		int operation = operations[i].second;

		switch ( types[index] ) {
			case 0:
				doOperation<CBase, CDerived0>( data, operation, index );
				break;
			case 1:
				doOperation<CBase, CDerived1>( data, operation, index );
				break;
			case 2:
				doOperation<CBase, CDerived2>( data, operation, index );
				break;
			case 3:
				doOperation<CBase, CDerived3>( data, operation, index );
				break;
			case 4:
				doOperation<CBase, CDerived4>( data, operation, index );
				break;
			default:
				break;
		}
	}
}

int main()
{
	std::uniform_int_distribution<int> getClass( MinClass, MaxClass );
	std::uniform_int_distribution<int> getOperation( 0, 2 );
	std::uniform_int_distribution<int> getIndex( 0, SampleCount - 1 );
	std::default_random_engine randomize;

	for ( int i = 0; i < SampleCount; ++i ) {
		types.push_back( getClass( randomize ) );
	}
	for ( int i = 0; i < OperationCount; ++i ) {
		operations.push_back( std::make_pair<int, int>( getIndex( randomize ), getOperation( randomize ) ) );
	}
	for ( int i = 0; i < SampleCount; ++i ) {
		classes[i] = nullptr;
		structs[i] = nullptr;
	}

	if ( sizeof( CClassM ) != sizeof( CStructM ) ) {
		return 0;
	}

	try {
		initHeapManager( 200000000 );

		auto start = std::chrono::high_resolution_clock::now();
		test<CClass, CClassXS, CClassS, CClassM, CClassL, CClassXL>( classes );
		auto middle = std::chrono::high_resolution_clock::now();
		test<CStruct, CStructXS, CStructS, CStructM, CStructL, CStructXL>( structs );
		auto finish = std::chrono::high_resolution_clock::now();

		__int64 dur1 = ( middle - start ).count();
		__int64 dur2 = ( finish - middle ).count();

		std::cout << dur1 << " " << dur2 << " " << static_cast<double>( dur1 ) / dur2 << std::endl;

		int enterSomethingToCloseProcess = 0;
		std::cin >> enterSomethingToCloseProcess;
		clearHeapManager();
	} catch ( const std::exception& exception ) {
		std::cerr << exception.what() << std::endl;
	}
	return 0;
}

#include <Windows.h>

#include <chrono> 
#include <iostream>
#include <random>

#include "HeapManager.h"
#include "Class.h"
#include "Struct.h"

const int minClass = 0;
const int maxClass = 3;
const int sampleCount = 50;
const int operationCount = 10000;

std::vector<int> types;

std::vector<std::pair<int, int>> operations;

CClass* classes[sampleCount];
CStruct* structs[sampleCount];

void testClass()
{
	for ( int i = 0; i < operationCount; ++i ) {
		int index = operations[i].first;
		int operation = operations[i].second;

		switch ( types[index] ) {
			case 0:
				if ( operation == 1 && classes[index] == nullptr ) {
					classes[index] = new CClassXS;
				} else if ( operation == 0 && classes[index] != nullptr ) {
					delete classes[index];
					classes[index] = nullptr;
				}
				break;

			case 1:
				if ( operation == 1 && classes[index] == nullptr ) {
					classes[index] = new CClassS;
				} else if ( operation == 0 && classes[index] != nullptr ) {
					delete classes[index];
					classes[index] = nullptr;
				}
				break;

			case 2:
				if ( operation == 1 && classes[index] == nullptr ) {
					classes[index] = new CClassM;
				} else if ( operation == 0 && classes[index] != nullptr ) {
					delete classes[index];
					classes[index] = nullptr;
				}
				break;

			case 3:
				if ( operation == 1 && classes[index] == nullptr ) {
					classes[index] = new CClassL;
				} else if ( operation == 0 && classes[index] != nullptr ) {
					delete classes[index];
					classes[index] = nullptr;
				}
				break;

			case 4:
				if ( operation == 1 && classes[index] == nullptr ) {
					classes[index] = new CClassXL;
				} else if ( operation == 0 && classes[index] != nullptr ) {
					delete classes[index];
					classes[index] = nullptr;
				}
				break;

			default:
				break;
		}
	}
}

void testStruct()
{
	for ( int i = 0; i < operationCount; ++i ) {
		int index = operations[i].first;
		int operation = operations[i].second;

		switch ( types[index] ) {
			case 0:
				if ( operation == 1 && structs[index] == nullptr ) {
					structs[index] = new CStructXS;
				} else if ( operation == 0 && structs[index] != nullptr ) {
					delete structs[index];
					structs[index] = nullptr;
				}
				break;

			case 1:
				if ( operation == 1 && structs[index] == nullptr ) {
					structs[index] = new CStructS;
				} else if ( operation == 0 && structs[index] != nullptr ) {
					delete structs[index];
					structs[index] = nullptr;
				}
				break;

			case 2:
				if ( operation == 1 && structs[index] == nullptr ) {
					structs[index] = new CStructM;
				} else if ( operation == 0 && structs[index] != nullptr ) {
					delete structs[index];
					structs[index] = nullptr;
				}
				break;

			case 3:
				if ( operation == 1 && structs[index] == nullptr ) {
					structs[index] = new CStructL;
				} else if ( operation == 0 && structs[index] != nullptr ) {
					delete structs[index];
					structs[index] = nullptr;
				}
				break;

			case 4:
				if ( operation == 1 && structs[index] == nullptr ) {
					structs[index] = new CStructXL;
				} else if ( operation == 0 && structs[index] != nullptr ) {
					delete structs[index];
					structs[index] = nullptr;
				}
				break;

			default:
				break;
		}
	}
}

int main()
{
	std::uniform_int_distribution<int> getClass( minClass, maxClass );
	std::uniform_int_distribution<int> getOperation( 0, 1 );
	std::uniform_int_distribution<int> getIndex( 0, sampleCount - 1 );
	std::default_random_engine randomize;

	for ( int i = 0; i < sampleCount; ++i ) {
		types.push_back( getClass( randomize ) );
	}
	for ( int i = 0; i < operationCount; ++i ) {
		operations.push_back( std::make_pair<int, int>( getIndex( randomize ), getOperation( randomize ) ) );
	}
	for ( int i = 0; i < sampleCount; ++i ) {
		classes[i] = nullptr;
		structs[i] = nullptr;
	}

	if ( sizeof( CClassM ) != sizeof( CStructM ) ) {
		return 0;
	}

	try {
		initHeapManager( 20000000 );
		auto start = std::chrono::high_resolution_clock::now();
		testClass();
		auto middle = std::chrono::high_resolution_clock::now();
		testStruct();
		auto finish = std::chrono::high_resolution_clock::now();
		__int64 dur1 = ( middle - start ).count();
		__int64 dur2 = ( finish - middle ).count();
		std::cout << dur1 << " " << dur2 << " " << static_cast<double>( dur1 ) / dur2 << std::endl;
		clearHeapManager();
	} catch ( const std::exception& exception ) {
		std::cerr << exception.what() << std::endl;
	}

	int enterSomethingToCloseProcess = 0;
	std::cin >> enterSomethingToCloseProcess;
	return 0;
}
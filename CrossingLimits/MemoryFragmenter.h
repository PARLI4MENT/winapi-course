#pragma once

#include <Windows.h>
#include <random>

void FragmentMemory()
{
	SIZE_T minBlockSize = 64 * 1024; // In bytes;

	std::default_random_engine randomize;
	std::uniform_int_distribution<int> getBlockSizeMultiplier( 50, 100 );
	std::uniform_int_distribution<int> getPositionDistribution( 0, minBlockSize * 32000 );

	for ( int i = 0, j = 0; i < 500 && j < 100000; ++j ) {
		SIZE_T blockSize = minBlockSize * getBlockSizeMultiplier( randomize );
		LPVOID position = ( LPVOID ) getPositionDistribution( randomize );
		LPVOID ptr = VirtualAlloc( position, blockSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );

		if ( ptr != NULL ) {
			++i;
		}
	}
}

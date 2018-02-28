#pragma once

#include <Windows.h>
#include <random>

void FragmentMemory()
{
	const SIZE_T minBlockSize = 64 * 1024; // In bytes;

	const std::uniform_int_distribution<int> getBlockSizeMultiplier( 50, 100 );
	const std::uniform_int_distribution<int> getPositionDistribution( 0, minBlockSize * 32000 );
	const std::uniform_int_distribution<int> getPageGuardFlag( 0, 1 );

	std::default_random_engine randomize;

	for ( int succeededCallsCount = 0, callsCount = 0; succeededCallsCount < 500 && callsCount < 100000; ++callsCount ) {
		const SIZE_T blockSize = minBlockSize * getBlockSizeMultiplier( randomize );
		const LPVOID position = ( LPVOID ) getPositionDistribution( randomize );
		const DWORD protectLevel = PAGE_READWRITE | ( PAGE_GUARD * getPageGuardFlag( randomize ) );

		if ( VirtualAlloc( position, blockSize, MEM_COMMIT | MEM_RESERVE, protectLevel ) != NULL ) {
			++succeededCallsCount;
		}
	}
}

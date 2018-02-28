#pragma once

#include <Windows.h>

#include <iostream>

#include "ErrorLogger.h"

void PrintMaxMemoryAndLastError()
{
	const SIZE_T blockSize = 64 * 1024; // In bytes.
	int allocatedBlocksCount = 0;

	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366887%28v=vs.85%29.aspx.
	while ( VirtualAlloc( NULL, blockSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE ) != NULL ) {
		++allocatedBlocksCount;
	}

	const int maxMemoryBytes = allocatedBlocksCount * blockSize;
	const double maxMemoryGigabytes = maxMemoryBytes / 1024.0 / 1024.0 / 1024.0;
	
	std::cout << allocatedBlocksCount << " blocks allocated, i. e. " << maxMemoryBytes << " b, " <<
		"or " << maxMemoryGigabytes << " Gb." << std::endl; // Expected nearly 32000, i. e. 2 Gb.

	Log();
}

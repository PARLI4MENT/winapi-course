#pragma once

#include <Windows.h>
#include <string>
#include <iostream>

// System error codes: https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx.

// This code is partly taken from the following pages:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms679351(v=vs.85).aspx;
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms680582(v=vs.85).aspx.
void PrintError()
{
	DWORD lastErrorCode = GetLastError(); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms679360(v=vs.85).aspx.
	LPTSTR lastErrorMessage = NULL;

	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms679351(v=vs.85).aspx.
	if ( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, lastErrorCode, 0, ( LPTSTR ) &lastErrorMessage, 0, NULL ) != 0 ) {
		std::wcout << ( LPTSTR ) lastErrorMessage << std::endl;
	} else {
		std::string errorMessage = "Can't format message for code " + std::to_string( lastErrorCode ) + ", failed with error " + std::to_string( GetLastError() ) + ".";
		throw std::exception( errorMessage.c_str() );
	}
}

void PrintMaxMemoryAndLastError()
{
	SIZE_T blockSize = 64 * 1024; // In bytes.
	int allocatedBlocksCount = 0;

	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366887%28v=vs.85%29.aspx.
	while ( VirtualAlloc( NULL, blockSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE ) != NULL ) {
		++allocatedBlocksCount;
	}

	int maxMemoryBytes = allocatedBlocksCount * blockSize;
	double maxMemoryGigabytes = maxMemoryBytes / 1024.0 / 1024.0 / 1024.0;
	std::cout << allocatedBlocksCount << " blocks allocated, i. e. " << maxMemoryBytes << " b, " <<
		"or " << maxMemoryGigabytes << " Gb." << std::endl; // Expected nearly 32000, i. e. 2 Gb.

	try {
		PrintError();
	} catch ( const std::exception& exception ) {
		std::cout << exception.what() << std::endl;
	} catch ( ... ) {
		std::cout << "Can't parse error." << std::endl;
	}
}

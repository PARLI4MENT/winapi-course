#pragma once

#include <Windows.h>

#include <iostream>
#include <string>

// System error codes: https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx.

// This code is partly taken from the following pages:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms679351(v=vs.85).aspx;
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms680582(v=vs.85).aspx.
void PrintError()
{
	const DWORD lastErrorCode = GetLastError(); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms679360(v=vs.85).aspx.

	LPTSTR lastErrorMessage = NULL;

	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms679351(v=vs.85).aspx.
	if ( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, lastErrorCode, MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ), ( LPTSTR ) &lastErrorMessage, 0, NULL ) != 0 ) {
		std::wcout << "Logger: last error - " << ( LPTSTR ) lastErrorMessage << std::endl;
	} else {
		std::string errorMessage = "Logger: Can't format message for code " + std::to_string( lastErrorCode ) + ", failed with error " + std::to_string( GetLastError() ) + ".";
		throw std::exception( errorMessage.c_str() );
	}
}

void Log()
{
	try {
		PrintError();
	} catch ( const std::exception& exception ) {
		std::cout << exception.what() << std::endl;
	} catch ( ... ) {
		std::cout << "Logger: Can't parse last error." << std::endl;
	}
}
#pragma once

#include <Windows.h>

#include <string>

void PrintMaxPathLength()
{
	const DWORD nBufferLength = 300;
	char lpBuffer[nBufferLength];
	const int currentDirectoryPathLength = ( int ) GetCurrentDirectory( nBufferLength, ( LPWSTR ) lpBuffer );

	if ( currentDirectoryPathLength == 0 ) {
		return;
	}

	int maxFileLength = 0;

	for ( std::wstring filename = std::to_wstring( 0 ); ; filename += std::to_wstring( 0 ) ) {
		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa363858%28v=vs.85%29.aspx (CreateFile),
		//  https://msdn.microsoft.com/en-us/library/windows/desktop/aa363915%28v=vs.85%29.aspx (DeleteFile).
		if ( CreateFile( ( LPCWSTR ) filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL ) == INVALID_HANDLE_VALUE ||
			DeleteFile( ( LPCWSTR ) filename.c_str() ) == FALSE ) {
			maxFileLength = filename.length();
			break;
		}
	}

	const int maxPathLenght = currentDirectoryPathLength + 1 + maxFileLength;

	std::cout << maxPathLenght << " is the length of the longest filename which can be used in WinAPI." << std::endl;
}

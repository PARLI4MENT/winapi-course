#pragma once

#include <Windows.h>

const int ProcessesCount = 4;

class CResourceManager {
public:
	STARTUPINFO StartupInfo{ sizeof( STARTUPINFO ) }; // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms686331(v=vs.85).aspx.
	PROCESS_INFORMATION ProcessInfo[ProcessesCount]; // See https://msdn.microsoft.com/en-us/library/windows/desktop/ms684873(v=vs.85).aspx.
	HANDLE Mapping[ProcessesCount];
	PVOID View[ProcessesCount];
	HANDLE AppearedEvents[ProcessesCount];
	HANDLE UpdatedEvents[ProcessesCount];

	~CResourceManager()
	{
		for( int i = 0; i < ProcessesCount; ++i ) {
			if( ProcessInfo[i].hThread != NULL ) {
				CloseHandle( ProcessInfo[i].hThread );
			}
			if( ProcessInfo[i].hProcess != NULL ) {
				CloseHandle( ProcessInfo[i].hProcess );
			}
			if( Mapping[i] != NULL ) {
				CloseHandle( Mapping[i] );
			}
			if( View[i] != NULL ) {
				UnmapViewOfFile( View[i] );
			}
			if( AppearedEvents[i] != NULL ) {
				CloseHandle( AppearedEvents[i] );
			}
			if( UpdatedEvents[i] != NULL ) {
				CloseHandle( AppearedEvents[i] );
			}
		}
	}
};

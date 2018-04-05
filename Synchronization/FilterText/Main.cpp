#include <Windows.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ResourceManager.h"

#include "FindWords.h"
#include "GetFileContent.h"
#include "Join.h"
#include "PutToFile.h"

std::vector<std::string> SplitWordsIntoParts( const std::vector<std::string>& words, const int partsCount )
{
	const int wordsCount = words.size();
	std::vector<std::string> parts;

	for( int i = 0; i < partsCount; ++i ) {
		auto start = words.begin() + (i * wordsCount) / partsCount;
		auto end = words.begin() + ( ( i + 1 ) * wordsCount ) / partsCount;
		parts.push_back( Join( std::vector<std::string>{ start, end } ) );
	}

	return parts;
}

CResourceManager ResourceManager;

void TryCreateProcess( const int i, const std::string& forbiddenWords )
{
	std::string command = "..\\Debug\\Worker.exe";
	// See https://msdn.microsoft.com/en-us/library/windows/desktop/ms682425(v=vs.85).aspx.
	BOOL isCreatingSussessful = CreateProcess( command.c_str(), const_cast<LPSTR>( forbiddenWords.c_str() ), NULL, NULL, FALSE, 0, NULL, NULL, &ResourceManager.StartupInfo, &( ResourceManager.ProcessInfo[i] ) );
	if( !isCreatingSussessful ) {
		throw std::runtime_error( "Cannot create process." );
	}
}

void TryCreateMapping( const int i )
{
	DWORD processId = ResourceManager.ProcessInfo[i].dwProcessId;
	std::string fileName = "file" + std::to_string( processId ) + ".txt";
	// See https://msdn.microsoft.com/en-us/library/windows/desktop/aa366537(v=vs.85).aspx.
	ResourceManager.Mapping[i] = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, /*TODO*/ 100500, fileName.c_str() );
	if( ResourceManager.Mapping[i] == NULL ) {
		throw std::runtime_error( "Cannot create file mapping." );
	}
	if( GetLastError() == ERROR_ALREADY_EXISTS ) {
		throw std::runtime_error( "The file already exists." );
	}
}

void TryMapViewOfFile( const int i )
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366761(v=vs.85).aspx.
	ResourceManager.View[i] = MapViewOfFile( ResourceManager.Mapping[i], FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0 );
	if( ResourceManager.View[i] == NULL ) {
		throw std::runtime_error( "Cannot map view of fied." );
	}
}

void WriteToViewOfFile( const int i, const char* text )
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366801.
	*static_cast< char** >( ResourceManager.View[i] ) = const_cast<char*>( text );
}

void TryCreateTextIsAppearedEvent( const int i )
{
	DWORD processId = ResourceManager.ProcessInfo[i].dwProcessId;
	std::string eventName = "TextIsAppeared" + std::to_string( processId );
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms682396(v=vs.85).aspx.
	ResourceManager.AppearedEvents[i] = CreateEvent( NULL, FALSE, TRUE, eventName.c_str() );
	if( ResourceManager.AppearedEvents[i] == NULL ) {
		throw std::runtime_error( "Cannot create event." );
	}
	if( GetLastError() == ERROR_ALREADY_EXISTS ) {
		throw std::runtime_error( "The event already exists." );
	}
}

void TryCreateTextIsUpdatedEvent( const int i )
{
	DWORD processId = ResourceManager.ProcessInfo[i].dwProcessId;
	std::string eventName = "TextIsUpdated" + std::to_string( processId );
	ResourceManager.UpdatedEvents[i] = CreateEvent( NULL, FALSE, FALSE, eventName.c_str() );
	if( ResourceManager.UpdatedEvents[i] == NULL ) {
		throw std::runtime_error( "Cannot create event." );
	}
	if( GetLastError() == ERROR_ALREADY_EXISTS ) {
		throw std::runtime_error( "The event already exists." );
	}
}

void ReadFromViewOfFile( const int i, std::string& text )
{
	text = std::string{ *static_cast<char**>( ResourceManager.View[i] ) };
}

int main( int argc, char** argv )
{
	if( argc != 4 ) {
		throw std::runtime_error( "Invalid arguments count." );
	}
	std::string targetWordsFileName = argv[1];
	std::string sourceFileName = argv[2];
	std::string targetFileName = argv[3];

	auto text = GetFileContent( sourceFileName );
	auto words = FindWords( text );
	auto parts = SplitWordsIntoParts( words, ProcessesCount );

	try {
		for( int i = 0; i < ProcessesCount; ++i ) {
			TryCreateProcess( i, targetWordsFileName );
			TryCreateMapping( i );
			TryMapViewOfFile( i );
			WriteToViewOfFile( i, parts[i].c_str() );
			UnmapViewOfFile( ResourceManager.View[i] ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366882(v=vs.85).aspx.
			TryCreateTextIsAppearedEvent( i );
			TryCreateTextIsUpdatedEvent( i );
		}
		WaitForMultipleObjects( ProcessesCount, ResourceManager.UpdatedEvents, TRUE, INFINITE ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms687025(v=vs.85).aspx.
		for( int i = 0; i < ProcessesCount; ++i ) {
			TryMapViewOfFile( i );
			ReadFromViewOfFile( i, parts[i] );
		}
	} catch( const std::runtime_error& exception ) {
		std::cerr << exception.what() << std::endl;
	}

	return 0;
}

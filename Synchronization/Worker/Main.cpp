#include <Windows.h>

#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>

#include "../FilterText/ResourceManager.h"

#include "../FilterText/FindWords.h"
#include "../FilterText/GetFileContent.h"
#include "../FilterText/Join.h"

std::vector<std::string> FilterWords( std::vector<std::string> words, const std::vector<std::string>& forbiddenWords )
{
	std::unordered_set<std::string> set{ words.begin(), words.end() };
	std::vector<std::string> filtered;
	std::copy_if( words.begin(), words.end(), std::back_inserter(filtered), [&] ( const std::string& word ) { return set.count( word ) > 0;  } );
	return filtered;
}

CResourceManager ResourceManager;

void TryCreateTextIsAppearedEvent( const int i = 0 )
{
	DWORD processId = GetCurrentProcessId();
	std::string eventName = "TextIsAppeared" + std::to_string( processId );
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms682396(v=vs.85).aspx.
	ResourceManager.AppearedEvents[i] = CreateEvent( NULL, FALSE, FALSE, eventName.c_str() );
	if( ResourceManager.AppearedEvents[i] == NULL ) {
		throw std::runtime_error( "Cannot create event." );
	}
	if( GetLastError() == ERROR_ALREADY_EXISTS ) {
		throw std::runtime_error( "The event already exists." );
	}
}

void TryCreateMapping( const int i = 0 )
{
	DWORD processId = GetCurrentProcessId();
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

void TryMapViewOfFile( const int i = 0 )
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366761(v=vs.85).aspx.
	ResourceManager.View[i] = MapViewOfFile( ResourceManager.Mapping[i], FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0 );
	if( ResourceManager.View[i] == NULL ) {
		throw std::runtime_error( "Cannot map view of fied." );
	}
}
void ReadFromViewOfFile( std::string& text, const int i = 0 )
{
	text = std::string{ *static_cast<char**>( ResourceManager.View[i] ) };
}

void UpdateText( std::string& text, const std::string& forbiddenWordsFileName )
{
	auto content = GetFileContent( forbiddenWordsFileName );
	auto forbiddenWords = FindWords( content );
	auto words = FindWords( text );
	auto filtered = FilterWords( words, forbiddenWords );
	text = Join( filtered );
}

void WriteToViewOfFile( const char* text, const int i = 0 )
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366801.
	*static_cast< char** >( ResourceManager.View[i] ) = const_cast<char*>( text );
}

void TryCreateTextIsUpdatedEvent( const int i = 0 )
{
	DWORD processId = ResourceManager.ProcessInfo[i].dwProcessId;
	std::string eventName = "TextIsUpdated" + std::to_string( processId );
	ResourceManager.UpdatedEvents[i] = CreateEvent( NULL, FALSE, TRUE, eventName.c_str() );
	if( ResourceManager.UpdatedEvents[i] == NULL ) {
		throw std::runtime_error( "Cannot create event." );
	}
	if( GetLastError() == ERROR_ALREADY_EXISTS ) {
		throw std::runtime_error( "The event already exists." );
	}
}

#include <iostream>
int main( const int argc, char** argv )
{
	if( argc != 2 ) {
		throw std::runtime_error( "Invalid arguments count." );
	}
	std::string forbiddenWordsFileName = argv[1];

	std::string text;

	TryCreateTextIsAppearedEvent();
	TryCreateMapping();
	TryMapViewOfFile();
	ReadFromViewOfFile( text );
	UnmapViewOfFile( ResourceManager.View[0] );
	UpdateText( text, forbiddenWordsFileName );
	WriteToViewOfFile( text.c_str() );
	TryCreateTextIsUpdatedEvent();

	return 0;
}
#include <Windows.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "../FilterText/ResourceManager.h"

#include "../FilterText/FindWords.h"
#include "../FilterText/GetFileContent.h"
#include "../FilterText/Join.h"

CResourceManager ResourceManager;

void TryCreateTextIsAppearedEvent( const int i = 0 )
{
	DWORD processId = GetCurrentProcessId();
	std::string eventName = "TextIsAppeared" + std::to_string( processId );
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms682396(v=vs.85).aspx.
	ResourceManager.TextIsAppearedEvents[i] = CreateEvent( NULL, FALSE, FALSE, eventName.c_str() );
	if( ResourceManager.TextIsAppearedEvents[i] == NULL ) {
		throw std::runtime_error( "Cannot create event." );
	}
}

void TryOpenMapping( const int i = 0 )
{
	DWORD processId = GetCurrentProcessId();
	std::string fileName = "file" + std::to_string( processId ) + ".txt";
	// See https://msdn.microsoft.com/en-us/library/windows/desktop/aa366791(v=vs.85).aspx.
	ResourceManager.Mapping[i] = OpenFileMapping( FILE_MAP_READ | FILE_MAP_WRITE, TRUE, fileName.c_str() );
	if( ResourceManager.Mapping[i] == NULL ) {
		throw std::runtime_error( "Cannot create file mapping." );
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
	text = static_cast<char*>( ResourceManager.View[i] );
}

std::vector<std::string> FilterWords( std::vector<std::string> words, const std::vector<std::string>& forbiddenWords )
{
	std::unordered_set<std::string> set{ forbiddenWords.begin(), forbiddenWords.end() };
	std::vector<std::string> filtered;
	std::copy_if( words.begin(), words.end(), std::back_inserter( filtered ), [&] ( const std::string& word ) { return set.count( word ) == 0;  } );
	return filtered;
}

void UpdateText( std::string& text, const std::string& forbiddenWordsFileName )
{
	auto content = GetFileContent( forbiddenWordsFileName );
	auto forbiddenWords = FindWords( content );
	auto words = FindWords( text );
	auto filtered = FilterWords( words, forbiddenWords );
	text = Join( filtered );
}

void WriteToViewOfFile( const std::string& text, const int i = 0 )
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366801.
	for( int j = 0; j < text.length(); ++j ) {
		static_cast<char*>( ResourceManager.View[i] )[j] = text[j];
	}
	static_cast<char*>( ResourceManager.View[i] )[text.length()] = '\0';
}

void TryCreateTextIsUpdatedEvent( const int i = 0 )
{
	DWORD processId = GetCurrentProcessId();
	std::string eventName = "TextIsUpdated" + std::to_string( processId );
	ResourceManager.TextIsUpdatedEvents[i] = CreateEvent( NULL, FALSE, FALSE, eventName.c_str() );
	if( ResourceManager.TextIsUpdatedEvents[i] == NULL ) {
		throw std::runtime_error( "Cannot create event." );
	}
	SetEvent( ResourceManager.TextIsUpdatedEvents[i] );
}

int main( const int argc, char** argv )
{
	if( argc != 2 ) {
		throw std::runtime_error( "Invalid arguments count." );
	}

	std::string forbiddenWordsFileName = argv[1];

	std::string text;

	TryCreateTextIsAppearedEvent();
	WaitForSingleObject( ResourceManager.TextIsAppearedEvents[0], INFINITY );
	TryOpenMapping();
	TryMapViewOfFile();
	ReadFromViewOfFile( text );
	UnmapViewOfFile( ResourceManager.View[0] );
	TryMapViewOfFile();
	UpdateText( text, forbiddenWordsFileName );
	WriteToViewOfFile( text.c_str() );
	TryCreateTextIsUpdatedEvent();

	return 0;
}

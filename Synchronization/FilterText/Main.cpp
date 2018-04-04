#include <Windows.h>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ExtractArguments.h"
#include "FindWords.h"
#include "GetFileContent.h"
#include "PutToFile.h"

const int ProcessesCount = 4;

std::string Join( const std::vector<std::string>& strings )
{
	std::ostringstream output{};
	std::ostream_iterator<std::string> iter( output, " " );
	std::copy( strings.begin(), strings.end(), iter );
	return output.str();
}

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

//std::string GetNewFileName()
//{
//	auto now = std::chrono::system_clock::now();
//	auto time = std::chrono::system_clock::to_time_t( now );
//	std::ostringstream stream;
//	stream << std::put_time( std::localtime( &time ), "%Y%m%Y%H%M%S" );
//	return stream.str();
//}

//std::vector<std::string> SplitFileIntoFiles( const std::string& fileName, const int filesCount )
//{
//	auto text = GetFileContent( fileName );
//	auto words = FindWords( text );
//	auto parts = SplitWordsIntoParts( words, filesCount );
//
//	std::vector<std::string> fileNames{};
//
//	for( const auto& part : parts ) {
//		//std::string fileName = GetNewFileName();
//		std::string fileName = "file";
//		fileNames.push_back( fileName );
//		PutToFile( fileName, part );
//	}
//
//	return fileNames;
//}

int main( int argc, char** argv )
{
	std::string targetWordsFileName;
	std::string sourceFileName;
	std::string targetFileName;
	ExtractArguments( argc, &argv, targetWordsFileName, sourceFileName, targetFileName );

	auto text = GetFileContent( sourceFileName );
	auto words = FindWords( text );
	auto parts = SplitWordsIntoParts( words, ProcessesCount );

	STARTUPINFO startupInfo = { sizeof( startupInfo ) }; // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms686331(v=vs.85).aspx.
	PROCESS_INFORMATION processInfo[ProcessesCount]; // See https://msdn.microsoft.com/en-us/library/windows/desktop/ms684873(v=vs.85).aspx.

	for( int i = 0; i < ProcessesCount; ++i ) {
		std::string command = "Worker.exe ../forbidden-words.txt";

		// See https://msdn.microsoft.com/en-us/library/windows/desktop/ms682425(v=vs.85).aspx.
		BOOL isCreatingSussessful = CreateProcess( NULL, const_cast<LPSTR>( command.c_str() ), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &(processInfo[i]));
		if( !isCreatingSussessful ) {
			throw std::runtime_error( "Cannot create process." );
		}

		DWORD processId = processInfo[i].dwProcessId;
		std::string fileName = "file" + std::to_string( processId ) + ".txt";

		// See https://msdn.microsoft.com/en-us/library/windows/desktop/aa366537(v=vs.85).aspx.
		HANDLE mapping = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, /*TODO*/ 100500, fileName.c_str() );
		if( mapping == NULL ) {
			throw std::runtime_error( "Cannot create file mapping." );
		}
		if( GetLastError() == ERROR_ALREADY_EXISTS ) {
			CloseHandle( mapping );
			throw std::runtime_error( "The file already exists." );
		}

		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366761(v=vs.85).aspx.
		PVOID view = MapViewOfFile( mapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0 );
		if( view == NULL ) {
			CloseHandle( mapping );
			throw std::runtime_error( "Cannot map view of filed." );
		}

		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366801.
		__try {
			*static_cast<char**>( view ) = const_cast<char*>( parts[i].c_str() );
		} __except( GetExceptionCode() == EXCEPTION_IN_PAGE_ERROR ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH ) {
			UnmapViewOfFile( view );
			CloseHandle( mapping );
			throw std::runtime_error( "Failed to write to the view." );
		}
		
		UnmapViewOfFile( view ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa366882(v=vs.85).aspx.
		CloseHandle( mapping ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms724211(v=vs.85).aspx.

		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms682396(v=vs.85).aspx.
		//HANDLE event = CreateEvent();
	}

	return 0;
}

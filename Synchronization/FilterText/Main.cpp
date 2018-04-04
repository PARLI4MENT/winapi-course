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

std::vector<std::vector<std::string>> SplitWordsIntoParts( const std::vector<std::string>& words, const int partsCount )
{
	const int wordsCount = words.size();
	std::vector<std::vector<std::string>> parts;

	for( int i = 0; i < partsCount; ++i ) {
		auto start = words.begin() + (i * wordsCount) / partsCount;
		auto end = words.begin() + ( ( i + 1 ) * wordsCount ) / partsCount;
		parts.push_back( std::vector<std::string>{ start, end } );
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

	STARTUPINFO startupInfo = { sizeof( startupInfo ) };
	PROCESS_INFORMATION processInfo[ProcessesCount];

	for( int i = 0; i < ProcessesCount; ++i ) {
		std::string command = "Worker.exe ../forbidden-words.txt";

		BOOL isCreatingSussessful = CreateProcess( NULL, const_cast<LPSTR>( command.c_str() ), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &(processInfo[i]));
		if( !isCreatingSussessful ) {
			std::runtime_error( "Cannot create process." );
		}

		DWORD processId = processInfo[i].dwProcessId;
		std::string fileName = "file" + std::to_string( processId ) + ".txt";

		//HANDLE crfl = CreateFile( fileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 

	}

	return 0;
}

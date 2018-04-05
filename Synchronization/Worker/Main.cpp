#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>

#include "../FilterText/ExtractArguments.h"
#include "../FilterText/FindWords.h"
#include "../FilterText/GetFileContent.h"
#include "../FilterText/PutToFile.h"

//std::vector<std::string> FilterWords( std::string words, const std::unordered_set<std::string>& forbiddenWords )
//{
//	std::vector<std::string> filtered;
//	std::copy_if( words.begin(), words.end(), std::back_inserter(filtered), [&] ( const std::string& word ) { return forbiddenWords.count( word ) > 0;  } );
//	return filtered;
//}

int main( const int argc, char** argv )
{
	if( argc != 2 ) {
		throw std::runtime_error( "Invalid arguments count." );
	}
	std::string forbiddenWordsFileName = argv[1];
	return 0;
}
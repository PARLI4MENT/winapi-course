#pragma once

#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

void PutToFile( const std::string& fileName, const std::vector<std::string>& words )
{
	std::ofstream output{ fileName, std::ios::out };
	std::ostream_iterator<std::string> iter{ output, " " };
	std::copy( words.begin(), words.end(), iter );
}

#pragma once

#include <iterator>
#include <sstream>
#include <string>

std::vector<std::string> FindWords( std::string text )
{
	std::istringstream stream{ text };
	std::vector<std::string> words{};
	std::copy( std::istream_iterator<std::string>( stream ),std::istream_iterator<std::string>(), std::back_inserter( words ) );
	return words;
}

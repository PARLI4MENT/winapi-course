#pragma once

#include <iterator>
#include <sstream>
#include <string>

std::vector<std::string> FindWords( std::string text )
{
	std::istringstream stream{ text };
	return std::vector<std::string>{std::istream_iterator<std::string>{stream}, std::istream_iterator<std::string>{}};
}

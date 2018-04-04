#pragma once

#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::string GetFileContent( std::string path )
{
	std::ifstream input{ path, std::ios::in };
	std::ostringstream stream{};
	stream << input.rdbuf();
	return stream.str();
}

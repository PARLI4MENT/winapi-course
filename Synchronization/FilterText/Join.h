#pragma once

#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::string Join( const std::vector<std::string>& strings )
{
	std::ostringstream output{};
	std::ostream_iterator<std::string> iter( output, " " );
	std::copy( strings.begin(), strings.end(), iter );
	return output.str();
}

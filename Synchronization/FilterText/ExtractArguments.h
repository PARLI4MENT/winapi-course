#pragma once

#include <stdexcept>
#include <string>

void ExtractArguments( int argc, char*** argv, std::string& fileName1 )
{
	if( argc != 2 ) {
		throw std::runtime_error( "Invalid arguments count." );
	}
	fileName1 = *argv[1];
}

void ExtractArguments( int argc, char*** argv, std::string& fileName1, std::string& fileName2, std::string& fileName3 )
{
	if( argc != 4 ) {
		throw std::runtime_error( "Invalid arguments count." );
	}
	fileName1 = *argv[1];
	fileName2 = *argv[2];
	fileName3 = *argv[3];
}

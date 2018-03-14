#pragma once

#include <exception>
#include <string>

class CException : std::exception {

public:
	CException( std::string _message );
	void PrintMessage();

private:
	std::string message;
};

//----------------------------------------------------------------------------------------------------------------------

class CIncorrectSize : CException {

public:
	CIncorrectSize( std::string _message );
};

//----------------------------------------------------------------------------------------------------------------------

class CVirtualAllocError : CException {

public:
	CVirtualAllocError( std::string _message );
};

//----------------------------------------------------------------------------------------------------------------------

class CVirtualFreeError : CException {

public:
	CVirtualFreeError( std::string _message );
};
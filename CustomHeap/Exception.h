#pragma once

#include <exception>
#include <string>

class CException : public std::exception {

public:
	CException( std::string _message );

	const char* what() const throw ( );

private:
	std::string message;
};

//----------------------------------------------------------------------------------------------------------------------

class CIncorrectSize : public CException {

public:
	CIncorrectSize( std::string _message );
};

//----------------------------------------------------------------------------------------------------------------------

class CVirtualAllocError : public CException {

public:
	CVirtualAllocError( std::string _message );
};

//----------------------------------------------------------------------------------------------------------------------

class CVirtualFreeError : public CException {

public:
	CVirtualFreeError( std::string _message );
};

//----------------------------------------------------------------------------------------------------------------------

class CMemoryLimitExceeded : public CException {

public:
	CMemoryLimitExceeded( std::string _message );
};

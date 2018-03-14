#include <iostream>

#include "Exception.h"

CException::CException( std::string _message ) :
	message( _message )
{
}

void CException::PrintMessage()
{
	std::cerr << message << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

CIncorrectSize::CIncorrectSize( std::string _message ) :
	CException( _message )
{
}

//----------------------------------------------------------------------------------------------------------------------

CVirtualAllocError::CVirtualAllocError( std::string _message ) :
	CException( _message )
{
}

//----------------------------------------------------------------------------------------------------------------------

CVirtualFreeError::CVirtualFreeError( std::string _message ) :
	CException( _message )
{
}

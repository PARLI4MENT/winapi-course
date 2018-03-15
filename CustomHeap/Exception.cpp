#include "Exception.h"

CException::CException( std::string _message ) :
	message( _message )
{
}

const char* CException::what() const throw( )
{
	return message.c_str();
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

//----------------------------------------------------------------------------------------------------------------------

CMemoryLimitExceeded::CMemoryLimitExceeded( std::string _message ) :
	CException( _message )
{
}

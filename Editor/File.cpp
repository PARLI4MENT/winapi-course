#include "File.h"

CFile::CFile( wchar_t* name )
{
	fileHandle = CreateFile( name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
}

CFile::~CFile()
{
	if( fileHandle != NULL ) {
		CloseHandle( fileHandle );
	}
}

BOOL CFile::Write( wchar_t* text, const int length )
{
	return WriteFile( fileHandle, text, length * sizeof( wchar_t ), NULL, NULL );
}

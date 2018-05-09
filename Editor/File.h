#pragma once

#include <Windows.h>

class CFile {
public:
	CFile() = default;
	CFile( wchar_t* name );
	~CFile();

	BOOL Write( wchar_t* text, const int length );

private:
	HANDLE fileHandle{};
};

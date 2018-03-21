#include <Windows.h>

#include <fstream>
#include <sstream>
#include <string>

#include "PalindromeCounterLibrary.h"

std::wstring GetPath()
{
	int argumentsCount;
	LPWSTR *arguments = CommandLineToArgvW( GetCommandLineW(), &argumentsCount );

	if ( arguments == 0 ) {
		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms645505(v=vs.85).aspx.
		MessageBoxW( 0, L"Unable to parse command line.", L"Error", MB_OK );
		std::terminate();
	}

	if ( argumentsCount != 2 ) {
		MessageBoxW( 0, L"Wrong command line arguments format.", L"Error", MB_OK );
		std::terminate();
	}

	std::wstring path{ arguments[1] };
	LocalFree( arguments );
	return path;
}

std::wstring GetFileContent( std::wstring path )
{
	std::wifstream input{ path, std::wios::in };
	std::wostringstream stream{};
	stream << input.rdbuf();
	return stream.str();
}

int WINAPI WinMain( HINSTANCE, HINSTANCE, LPTSTR, int )
{
	std::wstring path = GetPath();
	std::wstring text = GetFileContent( path );
	int palindromeCount = PalindromeCounter( text.c_str() );
	MessageBoxW( 0, std::to_wstring( palindromeCount ).c_str(), L"Info", MB_OK );
	return 0;
}

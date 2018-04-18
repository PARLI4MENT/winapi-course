#include <Windows.h>

#include "OverlappedWindow.h"

#include <string>

int wWinMain( HINSTANCE instanceHandle, HINSTANCE prevHandleInstance, LPWSTR commandLine, int windowShowMode )
{
	try {
		COverlappedWindow overlappedWindow{};

		if( !overlappedWindow.Register() ) {
			throw std::wstring{ L"Cannot register a window class." };
		}

		if( !overlappedWindow.Create() ) {
			throw std::wstring{ L"Cannot create a window." };
		}

		overlappedWindow.Show( windowShowMode );

		MSG message{};
		while( GetMessage( &message, ( HWND ) NULL, 0, 0 ) > 0 ) {
			TranslateMessage( &message );
			DispatchMessage( &message );
		}
	} catch( const std::wstring& errorMessage ) {
		MessageBoxW( 0, errorMessage.c_str() , L"Error", MB_OK );
	} catch( ... ) {
		MessageBoxW( 0, L"Unknown error", L"Error", MB_OK );
	}

	return 0;
}

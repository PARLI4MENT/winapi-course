#include <Windows.h>

#include "MainWindow.h"

#include <string>

int wWinMain( HINSTANCE instanceHandle, HINSTANCE prevHandleInstance, LPWSTR commandLine, int windowShowMode )
{
	try {
		CMainWindow mainWindow{};

		if( !mainWindow.Register() ) {
			throw std::wstring{ L"Cannot register a window class." };
		}

		if( !mainWindow.Create() ) {
			throw std::wstring{ L"Cannot create a window." };
		}

		mainWindow.Show( windowShowMode );

		MSG message{};
		while( GetMessage( &message, ( HWND ) NULL, 0, 0 ) > 0 ) {
			TranslateMessage( &message );
			DispatchMessage( &message );
		}
	} catch( const std::wstring& errorMessage ) {
		MessageBoxW( 0, errorMessage.c_str(), L"Error", MB_OK );
	} catch( ... ) {
		MessageBoxW( 0, L"Unknown error", L"Error", MB_OK );
	}

	return 0;
}

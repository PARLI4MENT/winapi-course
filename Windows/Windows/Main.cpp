#include <Windows.h>

#include "OverlappedWindow.h"

// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633559(v=vs.85).aspx.
int __stdcall wWinMain( HINSTANCE handleToInstance, HINSTANCE handleToPrevInstance, LPWSTR commandLine, int windowShowMode )
{
	try {
		COverlappedWindow overlappedWindow{};

		if( !overlappedWindow.Register() ) {
			throw L"Cannot register a window.";
		}

		if (!overlappedWindow.Create() ) {
			throw L"Cannot create a window.";
		}

		overlappedWindow.Show( windowShowMode );

		MSG message{};
		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644936(v=vs.85).aspx.
		while( GetMessage( &message, ( HWND ) NULL, 0, 0 ) > 0 ) {
			TranslateMessage( &message );
			DispatchMessage( &message );
		}
	} catch( const wchar_t* message ) {
		MessageBoxW( 0, message, L"Error", MB_OK );
	} catch( ... ) {
		MessageBoxW( 0, L"Unknown error", L"Error", MB_OK );
	}

	return 0;
}

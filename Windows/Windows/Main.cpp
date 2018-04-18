#include <Windows.h>

#include "OverlappedWindow.h"

// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633559(v=vs.85).aspx.
int wWinMain( HINSTANCE handleToInstance, HINSTANCE handleToPrevInstance, LPWSTR commandLine, int windowShowMode )
{
	try {
		COverlappedWindow overlappedWindow{};

		if( !overlappedWindow.Register() ) {
			throw std::wstring{ L"Cannot register a window class." };
		}

		if( !overlappedWindow.Create( std::wstring{ L"WindowTitle" } ) ) {
			throw std::wstring{ L"Cannot create a window." };
		}

		overlappedWindow.Show( windowShowMode );

		MSG message{};
		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644936(v=vs.85).aspx.
		while( GetMessage( &message, ( HWND ) NULL, 0, 0 ) > 0 ) {
			TranslateMessage( &message ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644955(v=vs.85).aspx.
			DispatchMessage( &message ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644934(v=vs.85).aspx.
		}
	} catch( const std::wstring& errorMessage ) {
		MessageBoxW( 0, errorMessage.c_str() , L"Error", MB_OK );
	} catch( ... ) {
		MessageBoxW( 0, L"Unknown error", L"Error", MB_OK );
	}

	return 0;
}

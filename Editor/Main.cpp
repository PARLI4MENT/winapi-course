#include <Windows.h>

#include "Editor.h"

#include <string>

int wWinMain( HINSTANCE instanceHandle, HINSTANCE prevHandleInstance, LPWSTR commandLine, int windowShowMode )
{
	try {
		CEditor editor{};

		if( !editor.Register() ) {
			throw std::wstring{ L"Не удалось зарегистрировать оконный класс." };
		}

		if( !editor.Create() ) {
			throw std::wstring{ L"Не удалось создать окно." };
		}

		editor.Show( windowShowMode );

		MSG message{};
		while( GetMessage( &message, ( HWND ) NULL, 0, 0 ) > 0 ) {
			TranslateMessage( &message );
			DispatchMessage( &message );
		}
	} catch( const std::wstring& errorMessage ) {
		MessageBoxW( 0, errorMessage.c_str(), L"Ошибка", MB_OK );
	} catch( ... ) {
		MessageBoxW( 0, L"Неизвестная ошибка.", L"Ошибка", MB_OK );
	}

	return 0;
}

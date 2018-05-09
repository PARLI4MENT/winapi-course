#include <Windows.h>

#include "Editor.h"

#include <string>

int wWinMain( HINSTANCE instanceHandle, HINSTANCE prevHandleInstance, LPWSTR commandLine, int windowShowMode )
{
	try {
		CEditor editor{};

		if( !editor.Register() ) {
			throw std::wstring{ L"�� ������� ���������������� ������� �����." };
		}

		if( !editor.Create() ) {
			throw std::wstring{ L"�� ������� ������� ����." };
		}

		editor.Show( windowShowMode );

		MSG message{};
		while( GetMessage( &message, ( HWND ) NULL, 0, 0 ) > 0 ) {
			TranslateMessage( &message );
			DispatchMessage( &message );
		}
	} catch( const std::wstring& errorMessage ) {
		MessageBoxW( 0, errorMessage.c_str(), L"������", MB_OK );
	} catch( ... ) {
		MessageBoxW( 0, L"����������� ������.", L"������", MB_OK );
	}

	return 0;
}

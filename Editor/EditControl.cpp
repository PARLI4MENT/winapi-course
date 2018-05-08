#include "EditControl.h"

#include <cstdlib>
#include <string>

CEditControl::~CEditControl()
{
	if( windowHandle != NULL ) {
		CloseHandle( windowHandle );
	}
}

bool CEditControl::Register()
{
	WNDCLASSEX windowClass{};

	windowClass.cbSize = sizeof( windowClass );
	windowClass.lpfnWndProc = windowProc;
	windowClass.hInstance = GetModuleHandle( NULL );
	windowClass.lpszClassName = L"EditControl";

	return RegisterClassEx( &windowClass ) != 0;
}

bool CEditControl::Create( HWND parentWinwowHandle, const int left, const int top, const int width, const int heigth )
{
	// Set lpParam to this in order to get it from lpCreateParams when receive WM_NCCREATE.
	return CreateWindowEx( 0, L"EditControl", L"EditControl", WS_CHILD | ES_MULTILINE,
		left, top, width, heigth, parentWinwowHandle, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
}

void CEditControl::Show( int windowShowMode )
{
	ShowWindow( windowHandle, windowShowMode );
}

void CEditControl::OnDestroy()
{
	PostQuitMessage( 0 );
}

void CEditControl::OnCreate()
{
	SetFocus( windowHandle );
}

void CEditControl::OnNCCreate( const HWND handle )
{
	windowHandle = handle;
}

LRESULT CEditControl::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) {
		case WM_DESTROY:
		{
			getThis( handle )->OnDestroy();
			return 0;
		}
		case WM_CREATE:
		{
			getThis( handle )->OnCreate();
			return DefWindowProc( handle, message, wParam, lParam );
		}
		case WM_NCCREATE:
		{
			// lParam "A pointer to the CREATESTRUCT structure that contains information about the window being created.
			// The members of CREATESTRUCT are identical to the parameters of the CreateWindowEx function."
			auto createStructPtr = reinterpret_cast< CREATESTRUCT* >( lParam );
			auto createParams = reinterpret_cast<LONG>( createStructPtr->lpCreateParams );

			// "...you should clear the last error information by calling SetLastError with 0 before calling SetWindowLong.
			// Then, function failure will be indicated by a return value of zero and a GetLastError result that is nonzero."
			SetLastError( 0 );
			if( SetWindowLong( handle, GWLP_USERDATA, createParams ) == 0 && GetLastError() != 0 ) {
				MessageBoxW( 0, std::to_wstring( GetLastError() ).c_str(), L"Error", NULL );
				return FALSE;
			}

			auto actualThis = reinterpret_cast<CEditControl*>( createParams );
			actualThis->OnNCCreate( handle );
			return DefWindowProc( handle, message, wParam, lParam );
		}
		default:
			return DefWindowProc( handle, message, wParam, lParam );
	}
}

CEditControl* CEditControl::getThis( HWND handle )
{
	return reinterpret_cast<CEditControl*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

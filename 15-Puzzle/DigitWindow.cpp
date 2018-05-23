#include "DigitWindow.h"

#include <cstdlib>
#include <string>

CDigitWindow::~CDigitWindow()
{
	if( windowHandle != NULL ) {
		CloseHandle( windowHandle );
	}
}

bool CDigitWindow::Register()
{
	WNDCLASSEX windowClass{};

	windowClass.cbSize = sizeof( windowClass );
	windowClass.lpfnWndProc = windowProc;
	windowClass.hInstance = GetModuleHandle( NULL );
	windowClass.hbrBackground = static_cast<HBRUSH>( GetStockObject( WHITE_BRUSH ) );
	windowClass.lpszClassName = L"DigitWindow";

	return RegisterClassEx( &windowClass ) != 0;
}

bool CDigitWindow::Create( HWND parentWinwowHandle, int left, int top, int width, int heigth, int _digit )
{
	digit = _digit;
	return CreateWindowEx( 0, L"DigitWindow", L"DigitWindow", WS_CHILD | WS_BORDER,
		left, top, width, heigth, parentWinwowHandle, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
}

void CDigitWindow::Show( int windowShowMode )
{
	ShowWindow( windowHandle, windowShowMode );
}

void CDigitWindow::OnDestroy()
{
	PostQuitMessage( 0 );
}

void CDigitWindow::OnCreate()
{
}

void CDigitWindow::OnNCCreate( const HWND handle )
{
	windowHandle = handle;
}

void CDigitWindow::OnPaint()
{
	PAINTSTRUCT paintStruct{};
	HDC paintDC = BeginPaint( windowHandle, &paintStruct );
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );
	DrawTextEx( paintDC, const_cast<LPWSTR>( std::to_wstring( digit ).c_str() ), -1, &rectangle, DT_CENTER | DT_VCENTER | DT_SINGLELINE, NULL );
	EndPaint( windowHandle, &paintStruct );
};

void CDigitWindow::OnLButtonDown()
{
	SetFocus( windowHandle );
}

LRESULT CDigitWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
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
			auto createStructPtr = reinterpret_cast< CREATESTRUCT* >( lParam );
			auto createParams = reinterpret_cast<LONG>( createStructPtr->lpCreateParams );

			SetLastError( 0 );
			if( SetWindowLong( handle, GWLP_USERDATA, createParams ) == 0 && GetLastError() != 0 ) {
				MessageBoxW( 0, std::to_wstring( GetLastError() ).c_str(), L"Error", NULL );
				return FALSE;
			}

			auto actualThis = reinterpret_cast<CDigitWindow*>( createParams );
			actualThis->OnNCCreate( handle );
			return DefWindowProc( handle, message, wParam, lParam );
		}
		case WM_PAINT:
		{
			getThis( handle )->OnPaint();
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			getThis( handle )->OnLButtonDown();
			return 0;
		}
		default:
			return DefWindowProc( handle, message, wParam, lParam );
	}
}

CDigitWindow* CDigitWindow::getThis( HWND handle )
{
	return reinterpret_cast<CDigitWindow*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

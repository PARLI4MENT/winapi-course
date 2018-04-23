#include "OverlappedWindow.h"

#include <cstdlib>
#include <string>

COverlappedWindow::~COverlappedWindow()
{
	if( windowHandle != NULL ) {
		CloseHandle( windowHandle );
	}
}

bool COverlappedWindow::Register()
{
	WNDCLASSEX windowClass{};

	windowClass.cbSize = sizeof( windowClass );
	windowClass.lpfnWndProc = windowProc;
	windowClass.hInstance = GetModuleHandle( NULL );
	windowClass.lpszClassName = L"OverlappedWindow";

	return RegisterClassEx( &windowClass ) != 0;
}

bool COverlappedWindow::Create()
{
	// Set lpParam to this in order to get it from lpCreateParams when receive WM_NCCREATE.
	return CreateWindowEx( 0, L"OverlappedWindow", L"OverlappedWindow", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
}

void COverlappedWindow::Show( int windowShowMode )
{
	ShowWindow( windowHandle, windowShowMode );
	for ( int i = 0; i < rowsCount; ++i ) {
		for( int j = 0; j < rowsCount; ++j ) {
			ellipseWindows[i][j].Show( windowShowMode );
		}
	}
}

void COverlappedWindow::OnDestroy()
{
	PostQuitMessage( 0 );
}

void COverlappedWindow::OnCreate()
{
	CEllipseWindow::Register();
	for( int i = 0; i < rowsCount; ++i ) {
		for( int j = 0; j < rowsCount; ++j ) {
			ellipseWindows[i][j].Create( windowHandle );
		}
	}
}

void COverlappedWindow::OnNCCreate( const HWND handle )
{
	windowHandle = handle;
}

LRESULT COverlappedWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) {
		case WM_DESTROY:
		{
			getThis( handle )->OnDestroy();
			break;
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

			auto actualThis = reinterpret_cast<COverlappedWindow*>( createParams );
			actualThis->OnNCCreate( handle );
			return DefWindowProc( handle, message, wParam, lParam );
		}
		default:
			return DefWindowProc( handle, message, wParam, lParam );
	}
	return 0;
}

COverlappedWindow* COverlappedWindow::getThis( HWND handle )
{
	return reinterpret_cast<COverlappedWindow*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

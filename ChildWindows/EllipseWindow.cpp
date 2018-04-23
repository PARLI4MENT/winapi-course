#include "EllipseWindow.h"

#include <cstdlib>
#include <string>

CEllipseWindow::~CEllipseWindow()
{
	if( windowHandle != NULL ) {
		CloseHandle( windowHandle );
	}
}

bool CEllipseWindow::Register()
{
	WNDCLASSEX windowClass{};

	windowClass.cbSize = sizeof( windowClass );
	windowClass.lpfnWndProc = windowProc;
	windowClass.hInstance = GetModuleHandle( NULL );
	windowClass.lpszClassName = L"EllipseWindow";

	return RegisterClassEx( &windowClass ) != 0;
}

bool CEllipseWindow::Create( HWND parentWinwowHandle )
{
	// Set lpParam to this in order to get it from lpCreateParams when receive WM_NCCREATE.
	return CreateWindowEx( 0, L"EllipseWindow", L"EllipseWindow", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parentWinwowHandle, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
}

void CEllipseWindow::Show( int windowShowMode )
{
	ShowWindow( windowHandle, windowShowMode );
}

void CEllipseWindow::OnDestroy()
{
	KillTimer( windowHandle, timer );
	PostQuitMessage( 0 );
}

void CEllipseWindow::OnCreate()
{
	timer = SetTimer( windowHandle, 0, 10, 0 ); 

	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );

	left = ( 2 * rectangle.left + rectangle.right ) / 3;
	top = ( 2 * rectangle.top + rectangle.bottom ) / 3;
	right = ( 2 * rectangle.right  + rectangle.left ) / 3;
	bottom = ( 2 * rectangle.bottom  + rectangle.top ) / 3;
}

void CEllipseWindow::OnNCCreate( const HWND handle )
{
	windowHandle = handle;
}

void CEllipseWindow::OnTimer()
{
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );
	InvalidateRect( windowHandle, &rectangle, FALSE );

	if( right + xMove > rectangle.right || left + xMove < rectangle.left ) {
		xMove *= -1;
	}
	if( bottom + yMove > rectangle.bottom || top + yMove < rectangle.top ) {
		yMove *= -1;
	}

	left += xMove;
	top += yMove;
	right += xMove;
	bottom += yMove;
}

void CEllipseWindow::OnPaint()
{
	PAINTSTRUCT paintStruct{};
	HDC paintDC = BeginPaint( windowHandle, &paintStruct );
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );

	HDC compatibleDC = CreateCompatibleDC( paintDC );
	HBITMAP compatibleBitmap = CreateCompatibleBitmap( paintDC, rectangle.right - rectangle.left, rectangle.bottom - rectangle.top );
	HGDIOBJ oldCompatibleBitmap = SelectObject( compatibleDC, compatibleBitmap );

	HBRUSH backgroundBrushHandle = CreateSolidBrush( RGB( 230, 230, 240 ) );
	SelectObject( compatibleDC, backgroundBrushHandle );
	Rectangle( compatibleDC, rectangle.left, rectangle.top, rectangle.right, rectangle.bottom );

	HBRUSH ellipseBrushHandle = CreateSolidBrush( RGB( 128, 128, 192 ) );
	SelectObject( compatibleDC, ellipseBrushHandle );
	Ellipse( compatibleDC, left, top, right, bottom );

	BitBlt( paintDC, rectangle.left, rectangle.top, rectangle.right - rectangle.left, rectangle.bottom - rectangle.top, compatibleDC, 0, 0, SRCCOPY );
	SelectObject( compatibleDC, oldCompatibleBitmap );

	DeleteObject( backgroundBrushHandle );
	DeleteObject( ellipseBrushHandle );
	DeleteObject( compatibleBitmap );
	DeleteDC( compatibleDC );
	EndPaint( windowHandle, &paintStruct );
}

LRESULT CEllipseWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
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

			auto actualThis = reinterpret_cast<CEllipseWindow*>( createParams );
			actualThis->OnNCCreate( handle );
			return DefWindowProc( handle, message, wParam, lParam );
		}
		case WM_TIMER: 
		{
			getThis( handle )->OnTimer();
			break;
		}
		case WM_PAINT:
		{
			getThis( handle )->OnPaint();
			break;
		}
		default:
			return DefWindowProc( handle, message, wParam, lParam );
	}
	return 0;
}

CEllipseWindow* CEllipseWindow::getThis( HWND handle )
{
	return reinterpret_cast<CEllipseWindow*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

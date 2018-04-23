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

bool CEllipseWindow::Create( HWND parentWinwowHandle, const int left, const int top, const int width, const int heigth )
{
	// Set lpParam to this in order to get it from lpCreateParams when receive WM_NCCREATE.
	return CreateWindowEx( 0, L"EllipseWindow", L"EllipseWindow", WS_CHILD | WS_BORDER,
		left, top, width, heigth, parentWinwowHandle, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
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

	if( GetFocus() == windowHandle ) {
		horizontalStep = left < rectangle.left ? std::abs( horizontalStep ) : ( right > rectangle.right ? -std::abs( horizontalStep ) : horizontalStep );
		verticalStep = top < rectangle.top ? std::abs( verticalStep ) : ( bottom > rectangle.bottom ? -std::abs( verticalStep ) : verticalStep );
		left += horizontalStep;
		top += verticalStep;
		right += horizontalStep;
		bottom += verticalStep;
	}
}

void CEllipseWindow::OnPaint()
{
	bool isFocused = GetFocus() == windowHandle;

	PAINTSTRUCT paintStruct{};
	HDC paintDC = BeginPaint( windowHandle, &paintStruct );
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );

	HDC compatibleDC = CreateCompatibleDC( paintDC );
	HBITMAP compatibleBitmap = CreateCompatibleBitmap( paintDC, rectangle.right - rectangle.left, rectangle.bottom - rectangle.top );
	HGDIOBJ oldCompatibleBitmap = SelectObject( compatibleDC, compatibleBitmap );

	HBRUSH backgroundBrushHandle = CreateSolidBrush( RGB( 230, 230, 240 ) );
	SelectObject( compatibleDC, backgroundBrushHandle );
	FillRect( compatibleDC, &rectangle, backgroundBrushHandle );

	HBRUSH ellipseBrushHandle = isFocused ? CreateSolidBrush( RGB( 128, 64, 64 ) ) : CreateSolidBrush( RGB( 128, 128, 192 ) );
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

void CEllipseWindow::onLButtonDown()
{
	SetFocus( windowHandle );
}

LRESULT CEllipseWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
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

			auto actualThis = reinterpret_cast<CEllipseWindow*>( createParams );
			actualThis->OnNCCreate( handle );
			return DefWindowProc( handle, message, wParam, lParam );
		}
		case WM_TIMER: 
		{
			getThis( handle )->OnTimer();
			return 0;
		}
		case WM_PAINT:
		{
			getThis( handle )->OnPaint();
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			getThis( handle )->onLButtonDown();
			return 0;
		}
		default:
			return DefWindowProc( handle, message, wParam, lParam );
	}
}

CEllipseWindow* CEllipseWindow::getThis( HWND handle )
{
	return reinterpret_cast<CEllipseWindow*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

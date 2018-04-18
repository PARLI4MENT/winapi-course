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
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633577(v=vs.85).aspx.
	WNDCLASSEX windowClass{};

	windowClass.cbSize = sizeof( windowClass );
	windowClass.lpfnWndProc = windowProc;
	windowClass.hInstance = GetModuleHandle( NULL ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms683199(v=vs.85).aspx.
	windowClass.lpszClassName = L"OverlappedWindow";

	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633587(v=vs.85).aspx.
	return RegisterClassEx( &windowClass ) != 0;
}

bool COverlappedWindow::Create()
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms632680(v=vs.85).aspx.
	// Set lpParam to this in order to get it from lpCreateParams when receive WM_NCCREATE.
	return CreateWindowEx( 0, L"OverlappedWindow", L"OverlappedWindow", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
}

void COverlappedWindow::Show( int windowShowMode )
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633548(v=vs.85).aspx.
	ShowWindow( windowHandle, windowShowMode );
}

void COverlappedWindow::OnDestroy()
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644945(v=vs.85).aspx.
	PostQuitMessage( 0 );
}

void COverlappedWindow::OnCreate()
{
}

void COverlappedWindow::OnNCCreate( const HWND handle )
{
	windowHandle = handle;
}

void COverlappedWindow::OnTimer()
{
}

void COverlappedWindow::OnPaint()
{
	PAINTSTRUCT paintStruct{};
	HDC paintDC = BeginPaint( windowHandle, &paintStruct ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/dd183362(v=vs.85).aspx.

	RECT rectangle{}; // See: https://msdn.microsoft.com/en-us/library/windows/desktop/dd162897(v=vs.85).aspx.
	GetClientRect( windowHandle, &rectangle ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633503(v=vs.85).aspx.

	HBRUSH backgroundBrushHandle = CreateSolidBrush( RGB( 230, 230, 240 ) ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/dd183518(v=vs.85).aspx,
	SelectObject( paintDC, backgroundBrushHandle ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/dd162957(v=vs.85).aspx.
	Rectangle( paintDC, rectangle.left, rectangle.top, rectangle.right, rectangle.bottom ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/dd162898(v=vs.85).aspx.
	DeleteObject( backgroundBrushHandle ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/dd183539(v=vs.85).aspx.

	HBRUSH ellipseBrushHandle = CreateSolidBrush( RGB( 128, 128, 192 ) );
	SelectObject( paintDC, ellipseBrushHandle );
	Ellipse( paintDC, 100, 100, 200, 400 ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/dd162510(v=vs.85).aspx.
	DeleteObject( ellipseBrushHandle );

	EndPaint( windowHandle, &paintStruct ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/dd162598(v=vs.85).aspx.
}

// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633573(v=vs.85).aspx.
LRESULT COverlappedWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) {
		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms632620(v=vs.85).aspx.
		case WM_DESTROY:
		{
			getThis( handle )->OnDestroy();
			break;
		}
		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms632619(v=vs.85).aspx.
		case WM_CREATE:
		{
			getThis( handle )->OnCreate();
			return DefWindowProc( handle, message, wParam, lParam );
		}
		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms632635(v=vs.85).aspx.
		case WM_NCCREATE:
		{
			// lParam "A pointer to the CREATESTRUCT structure that contains information about the window being created.
			// The members of CREATESTRUCT are identical to the parameters of the CreateWindowEx function."
			auto createStructPtr = reinterpret_cast< CREATESTRUCT* >( lParam ); // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms632603(v=vs.85).aspx.
			auto createParams = reinterpret_cast<LONG>( createStructPtr->lpCreateParams );

			// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633591(v=vs.85).aspx.
			// "...you should clear the last error information by calling SetLastError with 0 before calling SetWindowLong.
			// Then, function failure will be indicated by a return value of zero and a GetLastError result that is nonzero."
			SetLastError( 0 );
			if( SetWindowLong( handle, GWLP_USERDATA, createParams ) == 0 && GetLastError() != 0 ) {
				MessageBoxW( 0, std::to_wstring( GetLastError() ).c_str(), L"Error", NULL );
				std::abort();
			}

			auto actualThis = reinterpret_cast<COverlappedWindow*>( createParams );
			actualThis->OnNCCreate( handle );
			return DefWindowProc( handle, message, wParam, lParam );
		}
		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644902(v=vs.85).aspx.
		case WM_TIMER: 
		{
			getThis( handle )->OnTimer();
			break;
		}
		// See: https://msdn.microsoft.com/en-us/library/windows/desktop/dd145213(v=vs.85).aspx.
		case WM_PAINT:
		{
			getThis( handle )->OnPaint();
			break;
		}
		default:
			// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633572(v=vs.85).aspx.
			return DefWindowProc( handle, message, wParam, lParam );
	}
	return 0;
}

COverlappedWindow* COverlappedWindow::getThis( HWND handle )
{
	return reinterpret_cast<COverlappedWindow*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

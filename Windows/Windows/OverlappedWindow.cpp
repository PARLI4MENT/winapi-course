#include "OverlappedWindow.h"

HINSTANCE COverlappedWindow::instanceHandle;
const std::wstring COverlappedWindow::className = L"OverlappedWindow";

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
	windowClass.hInstance = instanceHandle;
	windowClass.lpszClassName = className.c_str();

	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633587(v=vs.85).aspx.
	return RegisterClassEx( &windowClass ) != 0;
}

bool COverlappedWindow::Create()
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms632680(v=vs.85).aspx.
	windowHandle = CreateWindowEx( 0, className.c_str(), L"Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, instanceHandle, NULL );
	return windowHandle != NULL;
}

void COverlappedWindow::Show( int windowShowMode )
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633548(v=vs.85).aspx.
	ShowWindow( windowHandle, windowShowMode );
}

void COverlappedWindow::OnDestroy()
{
	PostQuitMessage( 0 );
}

void COverlappedWindow::OnCreate()
{
}

void COverlappedWindow::OnNCCreate( const HWND handle )
{
}

void COverlappedWindow::OnTimer()
{
}

void COverlappedWindow::OnPaint()
{
}

LRESULT COverlappedWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) {
		//case WM_CREATE:
		case WM_PAINT:
			//case WM_SIZE:
			// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644945(v=vs.85).aspx.
			break;
		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;
		default:
			// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633572(v=vs.85).aspx.
			return DefWindowProc( handle, message, wParam, lParam );
	}
	return 0;
}

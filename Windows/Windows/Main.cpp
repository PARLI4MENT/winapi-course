#include <Windows.h>

LRESULT CALLBACK MainWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg ) {
		case WM_CREATE:	
		case WM_PAINT:
		case WM_SIZE:
		case WM_DESTROY:
			break;
		default:
			// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633572(v=vs.85).aspx.
			return DefWindowProc( hwnd, uMsg, wParam, lParam );
	}
	return 0;
}

int WinMain( HINSTANCE, HINSTANCE, LPTSTR, int )
{
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633577(v=vs.85).aspx.
	WNDCLASSEX windowClass;
	// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633587(v=vs.85).aspx.
	ATOM const atom = ::RegisterClassEx( &windowClass );
	return 0;
}

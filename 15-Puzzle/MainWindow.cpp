#include "MainWindow.h"

#include <algorithm>
#include <cstdlib>
#include <string>

CMainWindow::~CMainWindow()
{
	if( windowHandle != NULL ) {
		CloseHandle( windowHandle );
	}
}

bool CMainWindow::Register()
{
	WNDCLASSEX windowClass{};

	windowClass.cbSize = sizeof( windowClass );
	windowClass.lpfnWndProc = windowProc;
	windowClass.hInstance = GetModuleHandle( NULL );
	windowClass.lpszClassName = L"15-Puzzle";

	return RegisterClassEx( &windowClass ) != 0;
}

bool CMainWindow::Create()
{
	return CreateWindowEx( 0, L"15-Puzzle", L"15-Puzzle", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
}

void CMainWindow::Show( int windowShowMode )
{
	ShowWindow( windowHandle, windowShowMode );
	for( int i = 0; i < rowsCount; ++i ) {
		for( int j = 0; j < columnsCount; ++j ) {
			digitWindows[i][j].Show( windowShowMode );
		}
	}
}

void CMainWindow::OnDestroy()
{
	PostQuitMessage( 0 );
}

void CMainWindow::OnCreate()
{
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );

	const int width = ( rectangle.right - rectangle.left ) / columnsCount;
	const int height = ( rectangle.bottom - rectangle.top ) / rowsCount;

	CDigitWindow::Register();
	for( int i = 0; i < rowsCount; ++i ) {
		for( int j = 0; j < columnsCount; ++j ) {
			const int left = rectangle.left + j * width;
			const int top = rectangle.top + i * height;

			digitWindows[i][j].Create( windowHandle, left, top, width, height );
		}
	}
}

void CMainWindow::OnNCCreate( const HWND handle )
{
	windowHandle = handle;
}

void CMainWindow::OnSize()
{
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );

	const int width = ( rectangle.right - rectangle.left ) / columnsCount;
	const int height = ( rectangle.bottom - rectangle.top ) / rowsCount;

	for( int i = 0; i < rowsCount; ++i ) {
		for( int j = 0; j < columnsCount; ++j ) {
			const int left = rectangle.left + j * width;
			const int top = rectangle.top + i * height;

			SetWindowPos( digitWindows[i][j].windowHandle, HWND_TOP, left, top, width, height, SWP_SHOWWINDOW );
		}
	}
}

void CMainWindow::OnSizing( WPARAM wParam, RECT* rect )
{
	auto heigth = rect->bottom - rect->top;
	auto width = rect->right - rect->left;
	auto size = min( heigth, width );

	switch( wParam ) {
		case WMSZ_BOTTOM:
		case WMSZ_TOP:
			rect->right = rect->left + heigth;
			break;
		case WMSZ_LEFT:
		case WMSZ_RIGHT:
			rect->bottom = rect->top + width;
			break;
		case WMSZ_BOTTOMLEFT:
			rect->bottom = rect->top + size;
			rect->left = rect->right - size;
			break;
		case WMSZ_BOTTOMRIGHT:
			rect->bottom = rect->top + size;
			rect->right = rect->left + size;
			break;
		case WMSZ_TOPLEFT:
			rect->top = rect->bottom - size;
			rect->left = rect->right - size;
			break;
		case WMSZ_TOPRIGHT:
			rect->top = rect->bottom - size;
			rect->right = rect->left + size;
			break;
	}
}

LRESULT CMainWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
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
			auto createStructPtr = reinterpret_cast< CREATESTRUCT* >( lParam );
			auto createParams = reinterpret_cast<LONG>( createStructPtr->lpCreateParams );

			SetLastError( 0 );
			if( SetWindowLong( handle, GWLP_USERDATA, createParams ) == 0 && GetLastError() != 0 ) {
				MessageBoxW( 0, std::to_wstring( GetLastError() ).c_str(), L"Error", NULL );
				return FALSE;
			}

			auto actualThis = reinterpret_cast<CMainWindow*>( createParams );
			actualThis->OnNCCreate( handle );
			return DefWindowProc( handle, message, wParam, lParam );
		}
		case WM_SIZE:
		{
			getThis( handle )->OnSize();
			return TRUE;
		}
		case WM_SIZING:
		{
			getThis( handle )->OnSizing( wParam, reinterpret_cast<RECT*>( lParam ) );
			return 0;
		}
		default:
			return DefWindowProc( handle, message, wParam, lParam );
	}
	return 0;
}

CMainWindow* CMainWindow::getThis( HWND handle )
{
	return reinterpret_cast<CMainWindow*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

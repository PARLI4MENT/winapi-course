#include "DigitWindow.h"
#include "MainWindow.h"

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

bool CDigitWindow::Create( HWND _parentWindowHandle, int left, int top, int width, int heigth, CMainWindow* _parent, int _row, int _column )
{
	parentWindowHandle = _parentWindowHandle;
	parent = _parent;
	row = _row;
	column = _column;
	return CreateWindowEx( 0, L"DigitWindow", L"DigitWindow", WS_CHILD | WS_BORDER,
		left, top, width, heigth, parentWindowHandle, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
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

	bool isFocused = GetFocus() == windowHandle;
	auto backgroundColor = isFocused ? RGB( 192, 192, 208 ) : RGB( 255, 255, 255 );

	HBRUSH backgroundBrush = CreateSolidBrush( backgroundColor );
	FillRect( paintDC, &rectangle, backgroundBrush );
	DeleteObject( backgroundBrush );

	SetBkColor( paintDC, backgroundColor );
	auto text = std::to_wstring( parent->digits[row][column] );
	DrawText( paintDC, text.c_str(), -1, &rectangle, DT_CENTER | DT_SINGLELINE | DT_VCENTER );

	EndPaint( windowHandle, &paintStruct );
};

void CDigitWindow::OnLButtonDown()
{
	int blankRow = parent->blankRow;
	int blankColumn = parent->blankColumn;

	int rowDifference = std::abs( row - blankRow );
	int columnDifference = std::abs( column - blankColumn );

	if( rowDifference == 1 && columnDifference == 0 || rowDifference == 0 && columnDifference == 1 ) {
		std::swap( parent->digits[row][column], parent->digits[blankRow][blankColumn] );
		parent->blankRow = row;
		parent->blankColumn = column;

		SetFocus( windowHandle );
		InvalidateRect( parentWindowHandle, NULL, TRUE );
	}
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

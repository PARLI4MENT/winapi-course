#include "Editor.h"

#include <cstdlib>
#include <string>

CEditor::~CEditor()
{
	if( windowHandle != NULL ) {
		CloseHandle( windowHandle );
	}
}

bool CEditor::Register()
{
	WNDCLASSEX windowClass{};

	windowClass.cbSize = sizeof( windowClass );
	windowClass.lpfnWndProc = windowProc;
	windowClass.hInstance = GetModuleHandle( NULL );
	windowClass.lpszClassName = L"Editor";

	return RegisterClassEx( &windowClass ) != 0;
}

bool CEditor::Create()
{
	// Set lpParam to this in order to get it from lpCreateParams when receive WM_NCCREATE.
	return CreateWindowEx( 0, L"Editor", L"Editor", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
}

void CEditor::Show( int windowShowMode )
{
	ShowWindow( windowHandle, windowShowMode );
	editControl.Show( windowShowMode );
}

void CEditor::OnDestroy()
{
	PostQuitMessage( 0 );
}

void CEditor::OnCreate()
{
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );

	const int width = rectangle.right - rectangle.left;
	const int height = rectangle.bottom - rectangle.top;

	editControl.Create( windowHandle, rectangle.left, rectangle.top, width, height );
}

void CEditor::OnNCCreate( const HWND handle )
{
	windowHandle = handle;
}

void CEditor::OnSize()
{
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );

	const int width = rectangle.right - rectangle.left;
	const int height = rectangle.bottom - rectangle.top;

	SetWindowPos( editControl.windowHandle, HWND_TOP, rectangle.left, rectangle.top, width, height, SWP_SHOWWINDOW );
}

void CEditor::OnClose()
{
	if( isEdited ) {
		MessageBoxW( 0, L"Are you sure that you would like to close the window?", L"Close", MB_OK );
	}
}

void CEditor::OnCommand( WPARAM wParam )
{

	if( HIWORD( wParam ) == EN_CHANGE ) {
		isEdited = true;
	}
}

LRESULT CEditor::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
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
				MessageBoxW( 0, std::to_wstring( GetLastError() ).c_str(), L"Error", MB_OK );
				return FALSE;
			}

			auto actualThis = reinterpret_cast<CEditor*>( createParams );
			actualThis->OnNCCreate( handle );
			return DefWindowProc( handle, message, wParam, lParam );
		}
		case WM_SIZE:
		{
			getThis( handle )->OnSize();
			return 0;
		}
		case WM_CLOSE:
		{
			getThis( handle )->OnClose();
			return DefWindowProc( handle, message, wParam, lParam );
		}
		case WM_COMMAND:
		{
			getThis( handle )->OnCommand( wParam );
			return DefWindowProc( handle, message, wParam, lParam );;
		}
		default:
			return DefWindowProc( handle, message, wParam, lParam );
	}
	return 0;
}

CEditor* CEditor::getThis( HWND handle )
{
	return reinterpret_cast<CEditor*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

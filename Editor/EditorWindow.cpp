#include "EditorWindow.h"
#include "File.h"
#include "resource.h"

#include <cstdlib>
#include <string>

CEditorWindow::~CEditorWindow()
{
	if( windowHandle != NULL ) {
		CloseHandle( windowHandle );
	}
}

bool CEditorWindow::Register()
{
	WNDCLASSEX windowClass{};

	windowClass.cbSize = sizeof( windowClass );
	windowClass.lpfnWndProc = windowProc;
	windowClass.hInstance = GetModuleHandle( NULL );
	windowClass.lpszClassName = L"Editor";
	windowClass.lpszMenuName = MAKEINTRESOURCE( IDR_MENU1 );

	return RegisterClassEx( &windowClass ) != 0;
}

bool CEditorWindow::Create()
{
	// Set lpParam to this in order to get it from lpCreateParams when receive WM_NCCREATE.
	return CreateWindowEx( 0, L"Editor", L"miv", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
}

void CEditorWindow::Show( int windowShowMode )
{
	ShowWindow( windowHandle, windowShowMode );
	editControl.Show( windowShowMode );
}

void CEditorWindow::OnDestroy()
{
	PostQuitMessage( 0 );
}

void CEditorWindow::OnCreate()
{
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );

	const int width = rectangle.right - rectangle.left;
	const int height = rectangle.bottom - rectangle.top;

	editControl.Create( windowHandle, rectangle.left, rectangle.top, width, height );
}

void CEditorWindow::OnNCCreate( const HWND handle )
{
	windowHandle = handle;
}

void CEditorWindow::OnSize()
{
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );

	const int width = rectangle.right - rectangle.left;
	const int height = rectangle.bottom - rectangle.top;

	SetWindowPos( editControl.windowHandle, HWND_TOP, rectangle.left, rectangle.top, width, height, SWP_SHOWWINDOW );
}

bool CEditorWindow::OnClose()
{
	if( isEdited ) {
		switch( MessageBoxW( 0, L"Сохранить изменения?", L"Закрыть", MB_YESNOCANCEL ) ) {
			case IDYES:
			{
				const int length = static_cast<int>( SendMessage( editControl.windowHandle, WM_GETTEXTLENGTH, 0, 0 ) );
				wchar_t* buffer = new wchar_t[length + 1];
				SendMessage( editControl.windowHandle, WM_GETTEXT, length + 1, reinterpret_cast<LPARAM>( buffer ) );

				wchar_t name[MAX_PATH + 1];
				OPENFILENAME openFileName{};
				openFileName.lStructSize = sizeof( openFileName );
				openFileName.hwndOwner = windowHandle;
				openFileName.lpstrFile = name;
				openFileName.lpstrFile[0] = '\0';
				openFileName.nMaxFile = sizeof( name );

				if( GetSaveFileName( &openFileName ) ) {
					CFile file{ name };
					if( file.Write( buffer, length ) != TRUE ) {
						MessageBoxW( 0, L"Не удалось записать в файл.", L"Ошибка", MB_OK );
						return false;
					}
				} else {
					return false;
				}
				return true;
			}
			case IDNO:
				return true;
			case IDCANCEL:
				return false;
		}
	}
	return true;
}

void CEditorWindow::OnCommand( WPARAM wParam )
{
	auto command = HIWORD( wParam );
	if( command == EN_CHANGE ) {
		isEdited = true;
	} else if( command == 0 &&  LOWORD( wParam ) == ID_VIEW_SETTINGS ) {
		auto dialogBox = DialogBox( NULL, MAKEINTRESOURCE( IDD_DIALOG1 ), windowHandle, static_cast<DLGPROC>( dialogProc ) );
	}
}

LRESULT CEditorWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
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
				MessageBoxW( 0, std::to_wstring( GetLastError() ).c_str(), L"Ошибка", MB_OK );
				return FALSE;
			}

			auto actualThis = reinterpret_cast<CEditorWindow*>( createParams );
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
			return getThis( handle )->OnClose() ? DefWindowProc( handle, message, wParam, lParam ) : 0;
		}
		case WM_COMMAND:
		{
			getThis( handle )->OnCommand( wParam );
			return DefWindowProc( handle, message, wParam, lParam );;
		}
		default:
			return DefWindowProc( handle, message, wParam, lParam );
	}
}

CEditorWindow* CEditorWindow::getThis( HWND handle )
{
	return reinterpret_cast<CEditorWindow*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

BOOL CEditorWindow::dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) {
		case WM_COMMAND:
		{
			auto command = LOWORD( wParam );
			EndDialog( handle, wParam );
			return TRUE;
		}
	}
	return FALSE;
}

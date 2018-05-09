#include "DialogWindow.h"
#include "EditorWindow.h"
#include "resource.h"

#include <Windows.h>

#include <string>

void CDialogWindow::Create( HWND _windowHandle, CEditorWindow* _editor )
{
	windowHandle = _windowHandle;
	editor = _editor;
}

void CDialogWindow::Show()
{
	if( windowHandle != NULL ) {
		response = DialogBoxParam( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDD_DIALOG1 ), windowHandle, static_cast<DLGPROC>( dialogProc ), reinterpret_cast<LPARAM>( this ) );
	}
}

bool CDialogWindow::IsOK()
{
	return response == IDOK;
}

void CDialogWindow::Apply()
{
	editor->settings = settings;
}

void CDialogWindow::OnInitDialog( HWND handle )
{
}

bool CDialogWindow::OnCommand( WORD command, WPARAM wParam )
{
	// Return false iff dialog should be closed after command.
	switch( command ) {
		case IDC_BUTTON3:
		{
			CHOOSECOLORW colorData{};
			colorData.lStructSize = sizeof( colorData );
			colorData.hwndOwner = windowHandle;
			colorData.rgbResult = settings.BackgroundColor;
			colorData.lpCustColors = customBackgroundColors;
			colorData.Flags = CC_ANYCOLOR;
			ChooseColor( static_cast<LPCHOOSECOLORW>( &colorData ) );
			settings.BackgroundColor = colorData.rgbResult;
			return true;
		}
		case IDC_BUTTON4:
		{
			CHOOSECOLORW colorData{};
			colorData.lStructSize = sizeof( colorData );
			colorData.hwndOwner = windowHandle;
			colorData.rgbResult = settings.FontColor;
			colorData.lpCustColors = customFontColors;
			colorData.Flags = CC_ANYCOLOR;
			ChooseColor( static_cast<LPCHOOSECOLORW>( &colorData ) );
			settings.FontColor = colorData.rgbResult;
			return true;
		}
		case IDOK:
			return false;
		case IDCANCEL:
			return false;
		default:
			return true;
	}
}

BOOL CDialogWindow::dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) {
		case WM_INITDIALOG:
		{
			auto actualThis = reinterpret_cast<CDialogWindow*>( lParam );
			auto createParams = reinterpret_cast<LONG>( actualThis );

			SetLastError( 0 );
			if( SetWindowLong( handle, GWLP_USERDATA, createParams ) == 0 && GetLastError() != 0 ) {
				MessageBoxW( 0, std::to_wstring( GetLastError() ).c_str(), L"Îøèáêà", MB_OK );
				return FALSE;
			}

			actualThis->OnInitDialog( handle );
			return TRUE;
		}
		case WM_COMMAND:
		{
			if( getThis( handle )->OnCommand( LOWORD( wParam ), wParam ) ) {
			} else {
				EndDialog( handle, wParam );
			}
			return TRUE;
		}
	}
	return FALSE;
}

CDialogWindow* CDialogWindow::getThis( HWND handle )
{
	return reinterpret_cast<CDialogWindow*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

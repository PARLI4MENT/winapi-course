#include "DialogWindow.h"
#include "EditorWindow.h"
#include "resource.h"

#include <Windows.h>
#include <Commctrl.h>

#include <string>

void CDialogWindow::Create( HWND _windowHandle, CEditorWindow* _editor )
{
	parentWindowHandle = _windowHandle;
	editor = _editor;
}

void CDialogWindow::Show()
{
	if( parentWindowHandle != NULL ) {
		if( isModal ) {
			DialogBoxParam( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDD_DIALOG1 ), parentWindowHandle, static_cast<DLGPROC>( dialogProc ), reinterpret_cast<LPARAM>( this ) );
		} else {
			CreateDialogParam( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDD_DIALOG1 ), parentWindowHandle, static_cast<DLGPROC>( dialogProc ), reinterpret_cast<LPARAM>( this ) );
			ShowWindow( windowHandle, SW_SHOW );
		}
	}
}

bool CDialogWindow::IsOK()
{
	return response == IDOK;
}

void CDialogWindow::OnInitDialog( HWND handle )
{
	windowHandle = handle;
	oldSettings = editor->settings;

	auto checkBox = GetDlgItem( windowHandle, IDC_CHECK1 );
	if( wysiwyg ) {
		SendMessage( checkBox, BM_SETCHECK, static_cast< WPARAM >( BST_CHECKED ), NULL );
	}

	auto fontSizeSlider = GetDlgItem( windowHandle, IDC_SLIDER1 );
	SendMessage( fontSizeSlider, TBM_SETRANGE, TRUE, MAKELPARAM(8, 72) );
	SendMessage( fontSizeSlider, TBM_SETPOS, TRUE, static_cast<LPARAM>( oldSettings.FontSize ) );

	auto windowOpacitySlider = GetDlgItem( windowHandle, IDC_SLIDER2 );
	SendMessage( windowOpacitySlider, TBM_SETRANGE, TRUE, MAKELPARAM( 0, 255 ) );
	SendMessage( windowOpacitySlider, TBM_SETPOS, TRUE, static_cast<LPARAM>( oldSettings.Opacity ) );
}

bool CDialogWindow::OnCommand( WORD command, WPARAM wParam )
{
	// Returns false iff dialog should be closed after command.
	switch( command ) {
		case IDC_BUTTON3:
		{
			chooseColor( customBackgroundColors, newSettings.BackgroundColor );
			break;
		}
		case IDC_BUTTON4:
		{
			chooseColor( customFontColors, newSettings.FontColor );
			break;
		}
		case IDC_CHECK1:
		{
			wysiwyg = IsDlgButtonChecked( windowHandle, IDC_CHECK1 ) == BST_CHECKED;
			break;
		}
		case IDOK:
		{
			applySettings( newSettings );
			return false;
		}
		case IDCANCEL:
		{
			applySettings( oldSettings );
			return false;
		}
	}
	applySettingsDependOnWysiwyg();
	return true;
}

void CDialogWindow::OnHScroll( LPARAM lParam )
{
	auto scrollBarControl = reinterpret_cast<HWND>( lParam );
	auto fontSizeSlider = GetDlgItem( windowHandle, IDC_SLIDER1 );
	auto windowOpacitySlider = GetDlgItem( windowHandle, IDC_SLIDER2 );
	if( scrollBarControl == NULL ) {
		return;
	} else if( scrollBarControl == fontSizeSlider ) {
		newSettings.FontSize = SendMessage( fontSizeSlider, TBM_GETPOS, 0, 0 );
	} else if( scrollBarControl == windowOpacitySlider ) {
		newSettings.Opacity = SendMessage( windowOpacitySlider, TBM_GETPOS, 0, 0 );
	}
	applySettingsDependOnWysiwyg();
}

void CDialogWindow::chooseColor( COLORREF* customColors, COLORREF& newColor )
{
	CHOOSECOLORW colorData{};
	colorData.lStructSize = sizeof( colorData );
	colorData.hwndOwner = windowHandle;
	colorData.lpCustColors = customColors;
	colorData.Flags = CC_ANYCOLOR;
	ChooseColor( static_cast< LPCHOOSECOLORW >( &colorData ) );
	newColor = colorData.rgbResult;
}

void CDialogWindow::applySettings( CAppearanceSettings& settings )
{
	editor->settings = settings;
	InvalidateRect( editor->editControl.windowHandle, NULL, TRUE );

	if( editor->settings.Font != NULL ) {
		DeleteObject( editor->settings.Font );
	}
	auto font = reinterpret_cast<HFONT>( SendMessage( parentWindowHandle, WM_GETFONT, 0, 0 ) );
	LOGFONT logFont{};
	GetObject( font, sizeof( LOGFONT ), reinterpret_cast<LPVOID>( &logFont ) );
	logFont.lfHeight = static_cast<LONG>( editor->settings.FontSize );
	editor->settings.Font = CreateFontIndirect( &logFont );
	SendMessage( editor->editControl.windowHandle, WM_SETFONT, reinterpret_cast<WPARAM>( editor->settings.Font ), TRUE );

	SetLayeredWindowAttributes( editor->windowHandle, RGB( 0, 0, 0 ), 255 - editor->settings.Opacity, LWA_ALPHA | LWA_COLORKEY );
}

void CDialogWindow::applySettingsDependOnWysiwyg()
{
	if( wysiwyg ) {
		applySettings( newSettings );
	} else {
		applySettings( oldSettings );
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
			if( !getThis( handle )->OnCommand( LOWORD( wParam ), wParam ) ) {
				EndDialog( handle, wParam );
			}
			return TRUE;
		}
		case WM_HSCROLL:
		{
			getThis( handle )->OnHScroll( lParam );
			return TRUE;

		}
	}
	return FALSE;
}

CDialogWindow* CDialogWindow::getThis( HWND handle )
{
	return reinterpret_cast<CDialogWindow*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

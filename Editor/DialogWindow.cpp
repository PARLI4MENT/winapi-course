#include "DialogWindow.h"
#include "resource.h"

#include <Windows.h>

#include <string>

void CDialogWindow::Create( HWND handle )
{
	windowHandle = handle;
}

void CDialogWindow::Show()
{
	if( windowHandle != NULL ) {
		MessageBoxW( 0, std::to_wstring( reinterpret_cast<int>( this ) ).c_str(), L"1", MB_OK );
		response = DialogBoxParam( GetModuleHandle( 0 ), MAKEINTRESOURCE( IDD_DIALOG1 ), windowHandle, dialogProc, reinterpret_cast<LPARAM>( this ) );
	}
}

bool CDialogWindow::IsOK()
{
	return response == IDOK;
}

void CDialogWindow::Apply()
{
}

void CDialogWindow::OnInitDialog( HWND handle )
{
	windowHandle = handle;
}

bool CDialogWindow::OnCommand( WORD command )
{
	return false;
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
			MessageBoxW( 0, std::to_wstring( reinterpret_cast<int>( getThis( handle ) ) ).c_str(), L"2", MB_OK );
			if( getThis( handle )->OnCommand( LOWORD( wParam ) ) ) {
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

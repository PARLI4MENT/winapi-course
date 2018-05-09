#include "DialogWindow.h"
#include "resource.h"

#include <Windows.h>

CDialogWindow::CDialogWindow( HWND windowHandle )
{
	if( windowHandle != NULL ) {
		response = DialogBox( NULL, MAKEINTRESOURCE( IDD_DIALOG1 ), windowHandle, static_cast< DLGPROC >( dialogProc ) );
	}
}

bool CDialogWindow::IsOK()
{
	return response == IDOK;
}

void CDialogWindow::Apply()
{
}

BOOL CDialogWindow::dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
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

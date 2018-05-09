#include "EditControl.h"

#include <string>

CEditControl::~CEditControl()
{
	if( windowHandle != NULL ) {
		CloseHandle( windowHandle );
	}
}

bool CEditControl::Create( HWND parentWinwowHandle, const int left, const int top, const int width, const int heigth )
{
	// Set lpParam to this in order to get it from lpCreateParams when receive WM_NCCREATE.
	return CreateWindowEx( 0, L"EDIT", NULL, WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
		left, top, width, heigth, parentWinwowHandle, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
}

void CEditControl::Show( int windowShowMode )
{
	ShowWindow( windowHandle, windowShowMode );
	SetFocus( windowHandle );
}

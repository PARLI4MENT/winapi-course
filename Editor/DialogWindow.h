#pragma once

#include <Windows.h>

class CDialogWindow {
public:
	CDialogWindow( HWND windowHandle = NULL );

	bool IsOK();
	void Apply();

private:
	INT_PTR response{ IDCANCEL };

	static BOOL CALLBACK dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
};

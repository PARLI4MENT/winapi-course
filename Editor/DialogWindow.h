#pragma once

#include <Windows.h>

class CDialogWindow {
public:
	CDialogWindow() = default;

	void Create( HWND handle );
	void Show();

	bool IsOK();
	void Apply();

	void OnInitDialog( HWND handle );
	bool OnCommand( WORD command, WPARAM wParam );

private:
	HWND windowHandle{};
	INT_PTR response{ IDCANCEL };

	static BOOL CALLBACK dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static CDialogWindow* getThis( HWND handle );
};

#pragma once

#include <Windows.h>

#include "AppearanceSettings.h"

class CEditorWindow;

class CDialogWindow {
public:
	CDialogWindow() = default;

	void Create( HWND _windowHandle, CEditorWindow* _editor );
	void Show();

	bool IsOK();
	void Apply();

	void OnInitDialog( HWND handle );
	bool OnCommand( WORD command, WPARAM wParam );

private:
	HWND windowHandle{};
	INT_PTR response{ IDCANCEL };
	COLORREF customBackgroundColors[16]{};
	COLORREF customFontColors[16]{};

	CEditorWindow* editor{ nullptr };
	CAppearanceSettings settings{};

	static BOOL CALLBACK dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static CDialogWindow* getThis( HWND handle );
};

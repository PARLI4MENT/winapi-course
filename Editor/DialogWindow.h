#pragma once

#include <Windows.h>

#include "AppearanceSettings.h"

class CEditorWindow;

class CDialogWindow {
public:
	CDialogWindow() = default;

	void Create( HWND _windowHandle, CEditorWindow* _editor );
	void Show();

protected:
	void OnInitDialog( HWND handle );
	bool OnCommand( WORD command, WPARAM wParam );
	void OnHScroll( LPARAM lParam );
	void OnDestroy();

private:
	friend CEditorWindow;

	static const bool isModal{ false };
	HWND parentWindowHandle{};
	HWND windowHandle{};
	INT_PTR response{ IDCANCEL };
	COLORREF customBackgroundColors[16]{};
	COLORREF customFontColors[16]{};
	CEditorWindow* editor{ nullptr };
	CAppearanceSettings oldSettings{};
	CAppearanceSettings newSettings{};
	bool wysiwyg{ false };

	void chooseColor( COLORREF* customColors, COLORREF& newColor );
	void applySettings( CAppearanceSettings& settings );
	void applySettingsDependOnWysiwyg();
	static BOOL CALLBACK dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static CDialogWindow* getThis( HWND handle );
};

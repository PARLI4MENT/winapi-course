#pragma once

#include <Windows.h>

#include "AppearanceSettings.h"
#include "DialogWindow.h"
#include "EditControl.h"

class CDialogWindow;

class CEditorWindow {
public:
	CEditorWindow() = default;
	~CEditorWindow();

	static bool Register();
	bool Create();
	void Show( int windowShowMode );

protected:
	void OnDestroy();
	void OnCreate();
	void OnNCCreate( const HWND handle );
	void OnSize();
	bool OnClose();
	void OnCommand( WPARAM wParam );
	HBRUSH OnCtlColorEdit( HDC hDC );

private:
	friend CDialogWindow;

	HWND windowHandle{};
	CEditControl editControl{};
	CDialogWindow dialogWindow{};
	CAppearanceSettings settings{};
	bool isEdited{ false };

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static CEditorWindow* getThis( HWND handle );
};

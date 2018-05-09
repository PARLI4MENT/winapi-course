#pragma once

#include <Windows.h>

#include "EditControl.h"

class CEditor {
public:
	CEditor() = default;
	~CEditor();

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

private:
	HWND windowHandle{};
	CEditControl editControl{};
	bool isEdited{ false };

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static CEditor* getThis( HWND handle );
	static BOOL CALLBACK dialogProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
};

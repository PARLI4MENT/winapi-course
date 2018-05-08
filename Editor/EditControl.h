#pragma once

#include <Windows.h>

class CEditor;

class CEditControl {
	friend CEditor;

public:
	CEditControl() = default;
	~CEditControl();

	static bool Register();
	bool Create( HWND parentWinwowHandle, const int left, const int top, const int width, const int heigth );
	void Show( int windowShowMode );

protected:
	void OnDestroy();
	void OnCreate();
	void OnNCCreate( const HWND handle );
	void OnCommand( WPARAM wParam );

private:
	HWND windowHandle{};
	bool isEdited{ false };

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static CEditControl* getThis( HWND handle );
};

#pragma once

#include <Windows.h>

class CEditorWindow;

class CEditControl {
	friend CEditorWindow;

public:
	CEditControl() = default;
	~CEditControl();

	bool Create( HWND parentWinwowHandle, const int left, const int top, const int width, const int heigth );
	void Show( int windowShowMode );

private:
	HWND windowHandle{};
};

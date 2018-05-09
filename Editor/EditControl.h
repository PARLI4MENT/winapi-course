#pragma once

#include <Windows.h>

class CEditor;

class CEditControl {
	friend CEditor;

public:
	CEditControl() = default;
	~CEditControl();

	bool Create( HWND parentWinwowHandle, const int left, const int top, const int width, const int heigth );
	void Show( int windowShowMode );

private:
	HWND windowHandle{};
};

#pragma once

#include <Windows.h>

class CEditorWindow;
class CDialogWindow;

class CEditControl {

public:
	CEditControl() = default;
	~CEditControl();

	bool Create( HWND parentWinwowHandle, const int left, const int top, const int width, const int heigth );
	void Show( int windowShowMode );

private:
	friend CEditorWindow;
	friend CDialogWindow;

	HWND windowHandle{};
};

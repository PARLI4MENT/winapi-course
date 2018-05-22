#pragma once

#include <Windows.h>

class CMainWindow;

class CDigitWindow {
	friend CMainWindow;

public:
	CDigitWindow() = default;
	~CDigitWindow();

	static bool Register();
	bool Create( HWND parentWinwowHandle, int left, int top, int width, int heigth, int _digit );
	void Show( int windowShowMode );

protected:
	void OnDestroy();
	void OnCreate();
	void OnNCCreate( const HWND handle );
	void OnPaint();
	void onLButtonDown();

private:
	HWND windowHandle{};
	int digit = 0;

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static CDigitWindow* getThis( HWND handle );
};

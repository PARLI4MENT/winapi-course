#pragma once

#include <Windows.h>

class COverlappedWindow;

class CEllipseWindow {
	friend COverlappedWindow;

public:
	CEllipseWindow() = default;
	~CEllipseWindow();

	static bool Register();
	bool Create( HWND parentWinwowHandle, const int left, const int top, const int width, const int heigth );
	void Show( int windowShowMode );

protected:
	void OnDestroy();
	void OnCreate();
	void OnNCCreate( const HWND handle );
	void OnTimer();
	void OnPaint();

private:
	HWND windowHandle{};
	UINT_PTR timer{};
	int left = 0;
	int top = 0;
	int right = 0;
	int bottom = 0;
	int xMove = 10;
	int yMove = 10;

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static CEllipseWindow* getThis( HWND handle );
};

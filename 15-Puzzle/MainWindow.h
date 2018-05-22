#pragma once

#include <Windows.h>

#include "DigitWindow.h"

#include <array>

class CMainWindow {
public:
	CMainWindow() = default;
	~CMainWindow();

	static bool Register();
	bool Create();
	void Show( int windowShowMode );

protected:
	void OnDestroy();
	void OnCreate();
	void OnNCCreate( const HWND handle );
	void OnSize();
	void OnSizing( WPARAM wParam, RECT* rect );

private:
	HWND windowHandle{};
	const static int rowsCount = 4;
	const static int columnsCount = 4;
	std::array<std::array<CDigitWindow, columnsCount>, rowsCount> digitWindows{};

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static CMainWindow* getThis( HWND handle );
};

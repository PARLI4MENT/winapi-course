#pragma once

#include <Windows.h>

#include "EllipseWindow.h"

#include <array>

class COverlappedWindow {
public:
	COverlappedWindow() = default;
	~COverlappedWindow();

	static bool Register();
	bool Create();
	void Show( int windowShowMode );

protected:
	void OnDestroy();
	void OnCreate();
	void OnNCCreate( const HWND handle );
	void OnSize();

private:
	HWND windowHandle{};
	const static int rowsCount = 1;
	const static int columnsCount = 1;
	std::array<std::array<CEllipseWindow, columnsCount>, rowsCount> ellipseWindows{};

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static COverlappedWindow* getThis( HWND handle );
};

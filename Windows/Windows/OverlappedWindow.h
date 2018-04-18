#pragma once

#include <Windows.h>

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
	void OnTimer();
	void OnPaint();

private:
	HWND windowHandle{};

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static COverlappedWindow* getThis( HWND handle );
};

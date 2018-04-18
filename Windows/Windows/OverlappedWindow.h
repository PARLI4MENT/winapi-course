#pragma once

#include <Windows.h>

#include <string>

class COverlappedWindow {
public:
	COverlappedWindow() = default;
	~COverlappedWindow();

	static bool Register();
	bool Create( const std::wstring& windowTitle );
	void Show( int windowShowMode );

protected:
	void OnDestroy();
	void OnCreate();
	void OnNCCreate( const HWND handle );
	void OnTimer();
	void OnPaint();

private:
	static HINSTANCE instanceHandle;
	static const std::wstring className;
	HWND windowHandle{};
	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
};

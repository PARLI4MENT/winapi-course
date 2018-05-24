#pragma once

#include <Windows.h>

#include "DigitWindow.h"

#include <array>
#include <vector>

class CMainWindow {
	friend class CDigitWindow;

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
	void OnCommand( WPARAM wParam );

private:
	HWND windowHandle{};
	const static int degree = 4;
	std::array<std::array<CDigitWindow, degree>, degree> digitWindows{};
	std::array<std::array<int, degree>, degree> digits{};
	int blankRow = degree - 1;
	int blankColumn = degree - 1;

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static CMainWindow* getThis( HWND handle );
	static std::vector<int> getRandomPermutation( int length );
	static bool isSolvableState( const std::vector<int>& permutation, int degree );
	static bool isFinishState( const std::vector<int>& permutation );
	static std::vector<int> getInitialState( int degree );
};

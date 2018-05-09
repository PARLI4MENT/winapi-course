#pragma once

#include <Windows.h>

class CBrush {
public:
	CBrush( HBRUSH brushHandle = NULL );
	~CBrush();

	HBRUSH BrushHandle{ NULL };
};

//----------------------------------------------------------------------------------------------------------------------

class CAppearanceSettings {
public:
	CAppearanceSettings() = default;

	LOGFONT font{};
	COLORREF FontColor{ RGB( 0, 0, 0 ) };
	COLORREF BackgroundColor{ RGB( 255, 255, 255 ) };
	CBrush BackgroundBrush{};
};

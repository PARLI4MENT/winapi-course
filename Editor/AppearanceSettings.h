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

	HFONT Font{ NULL };
	COLORREF FontColor{ RGB( 0, 0, 0 ) };
	COLORREF BackgroundColor{ RGB( 255, 255, 255 ) };
	int FontSize = 24;
	int Opacity = 0;
};

#include "AppearanceSettings.h"

CBrush::CBrush( HBRUSH brushHandle ) :
	BrushHandle{ brushHandle }
{
}

CBrush::~CBrush()
{
	if( BrushHandle != NULL ) {
		DeleteObject( BrushHandle );
	}
}

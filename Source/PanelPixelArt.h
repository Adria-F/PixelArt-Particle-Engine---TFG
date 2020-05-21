#ifndef __PANEL_PIXELART_H__
#define __PANEL_PIXELART_H__

#include "Panel.h"

class PanelPixelArt : public Panel
{
public:

	PanelPixelArt(const char* name);
	~PanelPixelArt() {}

	void SetFlags();
	void DrawContent();
	void OnResize();
};

#endif // !__PANEL_PIXELART_H__
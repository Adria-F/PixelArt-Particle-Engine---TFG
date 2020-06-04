#ifndef __PANEL_NODECANVAS_H__
#define __PANEL_NODECANVAS_H__

#include "Panel.h"
#include <map>

class CanvasNode;
class ImFont;

class PanelNodeCanvas : public Panel
{
public:

	PanelNodeCanvas(const char* name);
	~PanelNodeCanvas() {}

	void SetFlags();
	void DrawContent();

	void SaveSettings(JSON_Value* settings);
	void LoadSettings(JSON_Value* settings);

	void Reset();

private:

	float2 scrolling = { 0.0f,0.0f };
	bool showGrid = true;
	bool midClicked = false;

	int zoom = 100;
};

#endif // !__PANEL_NODECANVAS_H__
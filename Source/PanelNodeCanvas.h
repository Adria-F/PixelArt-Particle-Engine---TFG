#ifndef __PANEL_NODECANVAS_H__
#define __PANEL_NODECANVAS_H__

#include "Panel.h"

class CanvasNode;
class ImFont;

class PanelNodeCanvas : public Panel
{
public:

	PanelNodeCanvas(const char* name);
	~PanelNodeCanvas() {}

	void SetFlags();
	void DrawContent();

	void DrawNodeList(float2 spawnPos, int zoom);

private:

	float2 scrolling = { 0.0f,0.0f };
	bool showGrid = true;

	int zoom = 100;

	char node_filter[64];
};

#endif // !__PANEL_NODECANVAS_H__
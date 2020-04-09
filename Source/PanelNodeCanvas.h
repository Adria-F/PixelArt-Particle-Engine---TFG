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

private:

	float2 scrolling = { 0.0f,0.0f };
	bool showGrid = true;

	int zoom = 100;

	CanvasNode* hoveredNode = nullptr;
	CanvasNode* selectedNode = nullptr;
};

#endif // !__PANEL_NODECANVAS_H__
#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__

#include "Panel.h"

class PanelScene : public Panel
{
public:

	PanelScene(const char* name);
	~PanelScene() {}

	void SetFlags();
	void DrawContent();
	void OnResize();
};

#endif // !__PANEL_SCENE_H__
#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__

#include "Panel.h"

class PanelScene : public Panel
{
public:

	PanelScene(const char* name);
	~PanelScene() {}

	void DrawContent();
};

#endif // !__PANEL_SCENE_H__
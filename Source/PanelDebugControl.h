#ifndef __PANEL_DEBUGCONTROL_H__
#define __PANEL_DEBUGCONTROL_H__

#include "Panel.h"

class PanelDebugControl : public Panel
{
public:

	PanelDebugControl(const char* name);
	~PanelDebugControl() {}

	void DrawContent();
};

#endif // !__PANEL_DEBUGCONTROL_H__
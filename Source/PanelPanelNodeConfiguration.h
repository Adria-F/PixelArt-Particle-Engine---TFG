#ifndef __PANEL_NODECONFIGURATION_H__
#define __PANEL_NODECONFIGURATION_H__

#include "Panel.h"

class PanelNodeConfiguration : public Panel
{
public:

	PanelNodeConfiguration(const char* name);
	~PanelNodeConfiguration() {}

	void DrawContent();
};

#endif // !__PANEL_NODECONFIGURATION_H__
#ifndef __PANEL_NODECONFIGURATION_H__
#define __PANEL_NODECONFIGURATION_H__

#include "Panel.h"

class PanelNodeConfiguration : public Panel
{
public:

	PanelNodeConfiguration(const char* name);
	~PanelNodeConfiguration() {}

	void SetFlags();
	void DrawContent();

public:

	bool collapsed = false;
	float2 openedPosition = { math::inf, math::inf };
};

#endif // !__PANEL_NODECONFIGURATION_H__
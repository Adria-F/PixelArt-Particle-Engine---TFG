#ifndef __MODULENODECANVAS_H__
#define __MODULENODECANVAS_H__

#include "Module.h"
#include <list>

class CanvasNode;

class ModuleNodeCanvas : public Module
{
public:

	ModuleNodeCanvas(bool start_enabled = true);
	~ModuleNodeCanvas();

	bool Start();
	update_state Update(float dt);
	bool CleanUp();

public:

	std::list<CanvasNode*> nodes;
};

#endif // !__MODULENODECANVAS_H__
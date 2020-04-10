#ifndef __MODULENODECANVAS_H__
#define __MODULENODECANVAS_H__

#include "Module.h"
#include <list>

class CanvasNode;
class NodeConnection;

enum connectionState;

class ModuleNodeCanvas : public Module
{
public:

	ModuleNodeCanvas(bool start_enabled = true);
	~ModuleNodeCanvas();

	bool Start();
	update_state Update(float dt);
	update_state PostUpdate(float dt);
	bool CleanUp();

	void StartConnection(NodeConnection* connection);
	void RequestConnection(NodeConnection* node);
	void StopConnection();

public:

	std::list<CanvasNode*> nodes;

private:

	NodeConnection* connecting = nullptr;
	NodeConnection* connectionCallback = nullptr;
	bool connectionEnded = false;
};

#endif // !__MODULENODECANVAS_H__
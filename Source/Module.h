#ifndef __MODULE_H__
#define __MODULE_H__

#include "Globals.h"
#include "SDL/include/SDL.h"

class Module
{
private:

	bool enabled;

public:

	Module (bool start_anabled = true): enabled(start_anabled)
	{}
	virtual ~Module(){}

	virtual bool Init()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_state PreUpdate()
	{
		return UPDATE_CONTINUE;
	}
	virtual update_state Update()
	{
		return UPDATE_CONTINUE;
	}
	virtual update_state PostUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}
};

#endif // !__MODULE_H__
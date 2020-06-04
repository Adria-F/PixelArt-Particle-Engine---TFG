#ifndef __MODULE_H__
#define __MODULE_H__

#include "Globals.h"

class JSON_Value;

class Module
{
private:

	bool enabled;

public:

	Module (bool start_enabled = true): enabled(start_enabled)
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

	virtual update_state PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}
	virtual update_state Update(float dt)
	{
		return UPDATE_CONTINUE;
	}
	virtual update_state PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual void SaveSettings(JSON_Value* settings) {}
	virtual void LoadSettings(JSON_Value* settings) {}
};

#endif // !__MODULE_H__
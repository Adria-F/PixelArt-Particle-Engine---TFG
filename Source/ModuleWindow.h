#ifndef __MODULEWINDOW_H__
#define __MODULEWINDOW_H__

#include "Module.h"

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void OnResize(int width, int height);

public:

	SDL_Window* window = nullptr;
	SDL_Surface* screen_surface = nullptr;

	uint height = 0;
	uint width = 0;

	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool fullscreen_desktop = false;
};

#endif // !__MODULEWINDOW_H__
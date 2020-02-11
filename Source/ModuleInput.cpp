#include "Application.h"
#include "ModuleInput.h"

ModuleInput::ModuleInput(bool start_enabled): Module(start_enabled)
{
}

ModuleInput::~ModuleInput()
{
}

bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);
	SDL_Init(SDL_INIT_GAMECONTROLLER);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	return ret;
}

update_state ModuleInput::PreUpdate()
{
	bool quit = false;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}
	}

	if (quit)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");

	SDL_QuitSubSystem(SDL_INIT_EVENTS);

	return true;
}

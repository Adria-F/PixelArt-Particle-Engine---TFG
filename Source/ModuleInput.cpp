#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleGUI.h"

ModuleInput::ModuleInput(bool start_enabled): Module(start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE)*MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE)*MAX_MOUSE_BUTTONS);
}

ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(keyboard);
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

update_state ModuleInput::PreUpdate(float dt)
{
	BROFILER_CATEGORY("ModuleInput Pre-Update", Profiler::Color::Magenta);

	SDL_PumpEvents();
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);

	// KEYBOARD
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	//MOUSE
	for (int i = 0; i < MAX_MOUSE_BUTTONS; ++i)
	{
		if (buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouseDeltaX = mouseDeltaY = mouseWheel = 0;

	bool quit = false;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		App->gui->handleInput(&event);
		switch (event.type)
		{
		case SDL_MOUSEWHEEL:
				mouseWheel = event.wheel.y;
			break;

		case SDL_MOUSEMOTION:
			mouseX = event.motion.x;
			mouseY = event.motion.y;

			mouseDeltaX = event.motion.xrel;
			mouseDeltaY = event.motion.yrel;
			break;

		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:
		{
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				App->window->OnResize(event.window.data1, event.window.data2);
				App->camera->OnResize(event.window.data1, event.window.data2);
				App->render->OnResize(event.window.data1, event.window.data2);
			}
			break;
		}
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

KEY_STATE ModuleInput::GetKey(int id) const
{
	if (App->gui->UsingKeyboard())
		return KEY_IDLE;
	else
		return keyboard[id];
}

KEY_STATE ModuleInput::GetMouseButton(int id) const
{
	if (App->gui->UsingMouse())
		return KEY_IDLE;
	else
		return mouse_buttons[id];
}

int ModuleInput::GetMouseX() const
{
	return mouseX;
}

int ModuleInput::GetMouseY() const
{
	return mouseY;
}

int ModuleInput::GetMouseWheel() const
{
	return mouseWheel;
}

int ModuleInput::GetMouseDeltaX() const
{
	return mouseDeltaX;
}

int ModuleInput::GetMouseDeltaY() const
{
	return mouseDeltaY;
}
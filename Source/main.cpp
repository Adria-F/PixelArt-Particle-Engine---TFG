#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

#pragma comment( lib, "Brofiler/ProfilerCore32.lib")

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = nullptr;

int main(int argc, char** argv)
{
	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:
			// Application Creation
			App = new Application();
			state = MAIN_START;
			
			break;
		case MAIN_START:
			//Application Init
			if (!App->Init())
			{
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
			}

			break;
		case MAIN_UPDATE:
		{
			BROFILER_FRAME("PixelArt VFX Tool");
			// Application Update
			update_state update_return = App->Update();
			if (update_return == UPDATE_ERROR)
			{
				state = MAIN_EXIT;
			}
			else if (update_return == UPDATE_STOP)
			{
				state = MAIN_FINISH;
			}
		}

			break;
		case MAIN_FINISH:
			// Application CleanUp
			if (App->CleanUp())
			{
				main_return = EXIT_SUCCESS;
			}
			//else
				//ERROR

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	App = nullptr;

	return main_return;
}
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleGUI.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */

ModuleRender::ModuleRender(bool start_enabled): Module(start_enabled)
{
}

ModuleRender::~ModuleRender()
{
}

bool ModuleRender::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum error = glewInit();
	if (error != GL_NO_ERROR)
	{
		LOG("Error initializing glew! Error: %s\n", gluErrorString(error));
		ret = false;
	}

	if (ret == true)
	{
		LOG("Using Glew %s", glewGetString(GLEW_VERSION));

		LOG("Vendor: %s", glGetString(GL_VENDOR));
		LOG("Renderer: %s", glGetString(GL_RENDERER));
		LOG("OpenGL version supported %s", glGetString(GL_VERSION));
		LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		//Use Vsync
		if (SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix. Specify which matrix is the current matrix
		glMatrixMode(GL_PROJECTION);//Applies subsequent matrix operations to the projection matrix stack. (screen position)
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix.Specify which matrix is the current matrix(camera position)
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//specify implementation of The most correct, or highest quality, option should be chosen.
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);//Specifies the depth value used when the depth buffer is cleared.

		//Initialize clear color
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);//clear values for the color buffers. The initial values are all 0.


		//specify pixel arithmetic glBlendFunc(rgb source blending factors, rgb destination blending factors)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		/*GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		/*lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();*/

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		//lights[0].Active(true);
		//glEnable(GL_LIGHTING);
		
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	return ret;
}

update_state ModuleRender::PreUpdate()
{
	//Render lights

	return UPDATE_CONTINUE;
}

update_state ModuleRender::PostUpdate()
{
	//Draw Scene
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//Base Plane
	/*MPlane base_plane(0, 1, 0, 0);
	base_plane.axis = true;
	base_plane.Render();*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	App->gui->Draw();

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

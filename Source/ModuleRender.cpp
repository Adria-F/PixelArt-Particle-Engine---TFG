#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleGUI.h"
#include "ModuleCamera.h"
#include "Shader.h"
#include "ModuleParticles.h"

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

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

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
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//clear values for the color buffers. The initial values are all 0. //Gray: 0.2f

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
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
		glEnable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		//Generate frame buffer
		generateFrameBuffer(App->window->width, App->window->height);

		//Create Shader program
		defaultShader = new Shader("Shaders/default.vs", "Shaders/default.fs");
		pixelartShader = new Shader("Shaders/pixelart.vs", "Shaders/pixelart.fs");
	}

	return ret;
}

update_state ModuleRender::PreUpdate(float dt)
{
	//Render lights
	/*for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();*/

	return UPDATE_CONTINUE;
}

update_state ModuleRender::PostUpdate(float dt)
{
	//Draw Scene
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Activate default shader
	defaultShader->Use();

	//Send Uniforms
	defaultShader->sendMat4("projection", App->camera->getProjectionMatrix());
	defaultShader->sendMat4("view", App->camera->getViewMatrix());

	//Draw Scene
	glEnable(GL_BLEND);
	App->particles->DrawParticles();

	//Activate pixelart shader
	glDisable(GL_BLEND);
	pixelartShader->Use();
	pixelartShader->sendTexture("scene", texture);
	float4x4 matrix;
	matrix.Set(float4x4::FromTRS(vec(0.0,0.0,0.0), Quat::identity, vec(2.0,2.0,2.0)));
	pixelartShader->sendMat4("model", (float*)matrix.Transposed().v);
	pixelartShader->sendVec2("viewportSize", App->gui->sceneSize);
	pixelartShader->sendUint("pixelSize", pixelSize);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //Draw screen-filling rect

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	App->gui->Draw(); // Draw GUI

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

uint ModuleRender::generateVAO(uint verticesSize, float* vertices, uint indicesSize, uint* indices)
{
	uint retVAO;

	unsigned int VBO, EBO;
	glGenVertexArrays(1, &retVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(retVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return retVAO;
}

void ModuleRender::OnResize(int width, int height)
{
	generateFrameBuffer(width, height);
}

void ModuleRender::generateFrameBuffer(int width, int height)
{
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &pixelartTexture);

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	glGenTextures(1, &pixelartTexture);
	glBindTexture(GL_TEXTURE_2D, pixelartTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, pixelartTexture, 0);

	GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, DrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("Error while creating framebuffer");
	}

	glViewport(0, 0, width, height);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

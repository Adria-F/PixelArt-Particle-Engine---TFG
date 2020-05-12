#ifndef __GLOBALS_H__
#define __GLOBALS_H__

// Warning disabled ---
//#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
//#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>

#include "MathGeoLib/MathGeoLib.h"
#include "SDL/include/SDL.h"
#include "ImGui/imgui.h"
#include "ImGuizmo\ImGuizmo.h"

#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

typedef unsigned int uint;

#include "Color.h"
#include "JSONManager.h"

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

// Deletes a buffer
#define RELEASE( x ) \
    {                        \
    if( x != NULL )        \
	    {                      \
      delete x;            \
	  x = NULL;              \
	    }                      \
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
    {                              \
    if( x != NULL )              \
	    {                            \
      delete[] x;                \
	  x = NULL;                    \
	    }                            \
                              \
}

#define START_RNG_SEED() startRNGSeed()
void startRNGSeed();

//Float between 0 and 1
#define GET_RANDOM() getRandom()
float getRandom();

#define GET_RANDOM_BETWEEN(min, max) getRandomBetween(min, max)
int getRandomBetween(int min, int max);

#define GENERATE_UID() generateUID()
uint generateUID();

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define PI 3.14159265358979323846

enum update_state
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

#define DEFAULT_FONT_SIZE 13.0f
#define CANVAS_FONT_SIZE 17.0f
#define MAX_ZOOM 3.0f
#define MIN_ZOOM 0.5f
#define ZOOM_STEPS 0.1f

#define CONNECTION_OUTLINE_THICKNESS 1.0f
#define CONNECTIONTRIANGLE_SIZE 12.0f
#define CONNECTIONCIRCLE_SIZE 5.0f

//Particle Data Order
#define MAX_ENTITY_DATA 10

//Node Canvas objects
#define NODE_PADDING 10.0f
#define NODE_BOX_PADDING 5.0f

#define NODE_DEFAULT_WIDTH 185.0f
#define NODE_DEFAULT_HEIGHT 50.0f

#define NODE_BOX_MIN_HEIGHT 75.0f
#define NODE_BOX_SEPARATION 20.0f
#define BOX_CONTAINER_MARGIN 20.0f
#define NODE_BOX_ADD_BUTTON_HEIGHT 20.0f

// Files PATHS
#define PATHS_AMOUNT 2
#define ASSETS_FOLDER "Assets/"
#define PROJECTS_FOLDER "Projects/"

#endif // !__GLOBALS_H__
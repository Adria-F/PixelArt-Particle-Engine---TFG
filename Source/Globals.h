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

#include "Color.h"

typedef unsigned int uint;

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
#define CANVAS_FONT_SIZE 24.0f
#define MAX_ZOOM 3.0f
#define MIN_ZOOM 0.5f
#define ZOOM_STEPS 0.1f

#define CONNECTION_OUTLINE_THICKNESS 2.0f
#define CONNECTIONTRIANGLE_SIZE 12.0f

//Particle Data Order
#define MAX_ENTITY_DATA 10

#define GRAPH_NODE_WINDOW_PADDING 10.0f
#define GRAPH_LINK_RADIUS 5.0f
#define STANDARD_NODE_WIDTH 90
#define STANDARD_NODE_HEIGHT 45

#endif // !__GLOBALS_H__
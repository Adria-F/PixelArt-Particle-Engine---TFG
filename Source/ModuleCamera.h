#ifndef __MODULECAMERA_H__
#define __MODULECAMERA_H__

#include "Module.h"

#define PIXELS_PER_UNIT 100 //Orthographic default zoom

enum cameraType
{
	CAMERA_2D,
	CAMERA_3D
};

class ModuleCamera : public Module
{
public:
	ModuleCamera(bool start_enabled = true);
	~ModuleCamera();

	bool Init();
	update_state Update(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	float* getProjectionMatrix();
	float* getViewMatrix();
	void LookAt(const vec &Spot);
	vec getMovementFactor();
	void setAspectRatio(float aspectRatio);

public:

	vec X = { 1.0f,0.0f,0.0f };
	vec Y = { 0.0f,1.0f,0.0f };
	vec Z = { 0.0f,0.0f,1.0f };
	vec position = { 0.0f,0.0f,10.0f };
	vec reference = { 0.0f,0.0f,0.0f };

	Frustum perspectiveFrustum;
	Frustum orthographicFrustum;

	cameraType type = CAMERA_2D;
};

#endif // !__MODULECAMERA_H__
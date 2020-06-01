#include "Application.h"
#include "ModuleCamera.h"

#include "ModuleInput.h"
#include "ModuleGUI.h"

ModuleCamera::ModuleCamera(bool start_enabled): Module(start_enabled)
{
	type = CAMERA_2D;

	perspectiveFrustum.type = math::FrustumType::PerspectiveFrustum;

	position = { 0.0f,10.0f,10.0f };
	perspectiveFrustum.pos = position;
	perspectiveFrustum.front = { 0.0f,0.0f,-1.0f };
	perspectiveFrustum.up = { 0.0f,1.0f,0.0f };

	perspectiveFrustum.nearPlaneDistance = 0.1f;
	perspectiveFrustum.farPlaneDistance = 1000.0f;

	perspectiveFrustum.verticalFov = DEGTORAD * 90.0f;
	perspectiveFrustum.horizontalFov = 2.f * atanf(tanf(perspectiveFrustum.verticalFov*0.5f)* (16.0f / 9.0f));

	LookAt(reference);

	orthographicFrustum.type = math::FrustumType::OrthographicFrustum;

	orthographicFrustum.pos = { 0.0f, 0.0f, 1.0f };
	orthographicFrustum.front = { 0.0f,0.0f,-1.0f };
	orthographicFrustum.up = { 0.0f,1.0f,0.0f };

	orthographicFrustum.nearPlaneDistance = 0.1f;
	orthographicFrustum.farPlaneDistance = 1000.0f;

	//Orthographic width/height will be set at the beginning by the call OnResize()
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	return true;
}

update_state ModuleCamera::Update(float dt)
{
	if (App->gui->IsExportPanelActive() && dt >= 0.0f) //Do not update camera while exporting
		return UPDATE_CONTINUE;

	if (type == CAMERA_3D && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		vec newPos(0, 0, 0);
		float speed = 0.2f;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed *= 2;
		else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
			speed /= 2;

		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += perspectiveFrustum.front*speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= perspectiveFrustum.front*speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= perspectiveFrustum.WorldRight()*speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += perspectiveFrustum.WorldRight()*speed;

		position += newPos;
		reference += newPos;

		reference = position - getMovementFactor();	

		perspectiveFrustum.pos = position;
	}

	if (type == CAMERA_2D)
	{
		//Manage 2D camera movement
		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT && App->gui->clickedScene)
		{
			float speed = 0.25f;
			float2 mouseDelta = { App->input->GetMouseDeltaX()*0.01f, -App->input->GetMouseDeltaY()*0.01f };
			orthographicFrustum.pos += {mouseDelta.x * speed, mouseDelta.y * speed, 0.0f};
		}

		//Manage 2D camera zoom
		float wheelDelta = App->input->GetMouseWheel();
		if (wheelDelta != 0.0f && (App->gui->mouseOnScene || App->gui->mouseOnPixelScene || App->gui->mouseOnExportScene))
		{
			//Update zoom and clamp
			orthographicFrustum.pos.z += wheelDelta*0.1f;
			if (orthographicFrustum.pos.z < 0.1f) //Min zoom
				orthographicFrustum.pos.z = 0.1f;
			else if (orthographicFrustum.pos.z > 3.0f) //Max zoom
				orthographicFrustum.pos.z = 3.0f;

			//Calculate new frustum size using new zoom value
			orthographicFrustum.orthographicWidth = viewportSize.x / (PIXELS_PER_UNIT * orthographicFrustum.pos.z);
			orthographicFrustum.orthographicHeight = viewportSize.y / (PIXELS_PER_UNIT * orthographicFrustum.pos.z);
		}
	}

	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

void ModuleCamera::OnResize(int width, int height)
{
	float newAR = (float)width / (float)height;
	setAspectRatio(newAR);

	orthographicFrustum.orthographicWidth = width / (PIXELS_PER_UNIT * orthographicFrustum.pos.z);
	orthographicFrustum.orthographicHeight = height / (PIXELS_PER_UNIT * orthographicFrustum.pos.z);
	viewportSize = { (float)width, (float)height };
}

float* ModuleCamera::getProjectionMatrix()
{
	static float4x4 matrix;
	if (type == CAMERA_3D)
	{
		matrix = perspectiveFrustum.ProjectionMatrix();
		matrix.Transpose();
	}
	else
	{
		matrix = orthographicFrustum.ProjectionMatrix();
		//Add the position of the camera to the matrix
		matrix[0][3] = orthographicFrustum.pos.x;
		matrix[1][3] = orthographicFrustum.pos.y;
		matrix.Transpose();
	}

	return (float*)matrix.v;
}

float* ModuleCamera::getViewMatrix()
{
	static float4x4 matrix;
	if (type == CAMERA_3D)
	{
		matrix = perspectiveFrustum.ViewMatrix();
		matrix.Transpose();
	}
	else
	{
		matrix = float4x4::identity.Transposed();
	}

	return (float*)matrix.v;
}

void ModuleCamera::LookAt(const vec & Spot)
{
	reference = Spot;

	//caluclate direction to look
	vec dir = Spot - perspectiveFrustum.pos;

	//caluclate the new view matrix
	float3x3 viewMat = float3x3::LookAt(perspectiveFrustum.front, dir.Normalized(), perspectiveFrustum.up, vec(0.0f, 1.0f, 0.0f));

	//set new front and up for the frustum
	perspectiveFrustum.front = viewMat.MulDir(perspectiveFrustum.front).Normalized();
	perspectiveFrustum.up = viewMat.MulDir(perspectiveFrustum.up).Normalized();
}

vec ModuleCamera::getMovementFactor()
{
	int dx = -App->input->GetMouseDeltaX();
	int dy = -App->input->GetMouseDeltaY();

	vec newPosition = position - reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx*0.01f;

		Quat rotation = Quat::RotateY(DeltaX);
		perspectiveFrustum.front = rotation.Mul(perspectiveFrustum.front).Normalized();
		perspectiveFrustum.up = rotation.Mul(perspectiveFrustum.up).Normalized();
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy*0.01f;

		Quat rotation = Quat::RotateAxisAngle(perspectiveFrustum.WorldRight(), DeltaY);

		if (rotation.Mul(perspectiveFrustum.up).Normalized().y > 0.0f)
		{
			perspectiveFrustum.up = rotation.Mul(perspectiveFrustum.up).Normalized();
			perspectiveFrustum.front = rotation.Mul(perspectiveFrustum.front).Normalized();
		}
	}

	return -perspectiveFrustum.front * newPosition.Length();
}

void ModuleCamera::setAspectRatio(float aspectRatio)
{
	perspectiveFrustum.horizontalFov = 2.f * atanf(tanf(perspectiveFrustum.verticalFov*0.5f)*aspectRatio);
}

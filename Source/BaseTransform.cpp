#include "Application.h"
#include "BaseTransform.h"

#include "ModuleRender.h"
#include "Shader.h"
#include "ModuleCamera.h"

void BaseTransform::Execute(float dt)
{
	if (!billboard)
		return;

	vec cameraPos = App->camera->position;
	//caluclate direction to look
	vec dir = cameraPos - position;

	//caluclate the new rotation matrix
	float3x3 rotMat = float3x3::LookAt(vec(0.0f, 0.0f, 1.0f), dir.Normalized(), vec(0.0f, 1.0f, 0.0f), vec(0.0f, 1.0f, 0.0f));

	//set new rotation quaternion
	rotation = rotMat.ToQuat();
}

void BaseTransform::PrepareRender()
{
	float4x4 matrix;
	matrix.Set(float4x4::FromTRS(position, rotation, scale));
	matrix.Transpose();
	App->render->defaultShader->sendMat4("model", (float*)matrix.v);
}

ParticleData* BaseTransform::Copy(Particle* particle) const
{
	BaseTransform* ret = new BaseTransform(particle);

	ret->position = position;
	ret->rotation = rotation;
	ret->scale = scale;

	return ret;
}
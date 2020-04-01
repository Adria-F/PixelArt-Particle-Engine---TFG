#include "Application.h"
#include "BaseTransform.h"

#include "ModuleRender.h"
#include "Shader.h"
#include "ModuleCamera.h"

BaseTransform::BaseTransform(Particle* particle) : ParticleData(particle)
{
	
}

void BaseTransform::PrepareRender()
{
	float4x4 matrix;
	matrix.Set(float4x4::FromTRS(position, rotation, scale));
	matrix.Transpose();
	App->render->defaultShader->sendMat4("model", (float*)matrix.v);
}

void BaseTransform::LookCamera()
{
	if (!billboard)
		return;

	vec cameraPos = App->camera->position;
	//caluclate direction to look
	vec dir = cameraPos - position;

	//caluclate the new rotation matrix
	float3x3 rotMat = float3x3::LookAt(vec(0.0f, 0.0f, 1.0f), dir.Normalized(), vec(0.0f, 1.0f, 0.0f), vec(0.0f, 1.0f, 0.0f));

	vec front = dir.Normalized();
	

	/*vec front, up, right;
	front = (App->camera->position - position).Normalized();
	up = App->camera->frustum.up;
	right = Cross(front, up);
	float3x3 rotMat;
	rotMat.SetCol(0, up);
	rotMat.SetCol(1, right);
	rotMat.SetCol(2, front);*/
	//set new rotation quaternion
	rotation = rotMat.ToQuat();

	if (randomizeRotation) //Set if custom initial rotation
	{
		vec front = (App->camera->position - position).Normalized();
		rotation = Quat::RotateAxisAngle(front, angleZ * DEGTORAD)*rotation;
	}
}

ParticleData* BaseTransform::Copy(Particle* particle) const
{
	BaseTransform* ret = new BaseTransform(particle);

	ret->position = position;
	ret->rotation = rotation;
	ret->scale = scale;

	ret->randomizeRotation = randomizeRotation;
	ret->randomizeScale = randomizeScale;

	ret->randRotation1 = randRotation1;
	ret->randRotation2 = randRotation2;
	
	ret->randScaleX1 = randScaleX1;
	ret->randScaleX2 = randScaleX2;
	ret->randScaleY1 = randScaleY1;
	ret->randScaleY2 = randScaleY2;

	if (randomizeRotation)
		ret->RandomizeRotation();

	if (randomizeScale)
		ret->RandomizeScale();

	return ret;
}

void BaseTransform::SetRandomScale(bool random)
{
	randomizeScale = random;
}

void BaseTransform::SetRandomRotation(bool random)
{
	randomizeRotation = random;
}

void BaseTransform::RandomizeScale()
{
	scale.x = Lerp(randScaleX1, randScaleX2, GET_RANDOM());
	scale.y = Lerp(randScaleY1, randScaleY2, GET_RANDOM());
}

void BaseTransform::RandomizeRotation()
{
	angleZ = Lerp(randRotation1, randRotation2, GET_RANDOM());

	vec front = (App->camera->position - position).Normalized();
	rotation = Quat::RotateAxisAngle(front, angleZ * DEGTORAD)*rotation;
}
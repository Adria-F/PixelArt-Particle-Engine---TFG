#include "Application.h"
#include "BaseTransformParticleNode.h"

#include "ModuleRender.h"
#include "Shader.h"
#include "ModuleCamera.h"
#include "ModuleParticles.h"
#include "BaseTransformEmitterNode.h"
#include "MakeGlobalParticleNode.h"

BaseTransformParticleNode::BaseTransformParticleNode(Particle* particle) : EntityData(particle)
{
	//At the point we construct the base nodes, all the other must be already constructed and added
	if (particle->makeGlobal != nullptr && particle->makeGlobal->active)
	{
		position = particle->emitter->baseTransform->position; //If its global, take emitter position to manage it on its own
	}
}

void BaseTransformParticleNode::PrepareRender()
{
	float4x4 matrix;
	globalPostion = position;

	if (particle->makeGlobal == nullptr || !particle->makeGlobal->active) //Ignore emitter transform if make global node is active
	{
		globalPostion += particle->emitter->baseTransform->position;
	}
	matrix.Set(float4x4::FromTRS(globalPostion, rotation, scale));

	App->render->defaultShader->sendMat4("model", (float*)matrix.Transposed().v);
}

void BaseTransformParticleNode::LookCamera()
{
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
}

EntityData* BaseTransformParticleNode::Copy(Particle* particle) const
{
	BaseTransformParticleNode* ret = new BaseTransformParticleNode(particle);

	ret->rotation = rotation;
	ret->scale = scale;

	return ret;
}
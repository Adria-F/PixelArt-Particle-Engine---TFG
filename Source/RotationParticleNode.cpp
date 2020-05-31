#include "Application.h"
#include "RotationParticleNode.h"

#include "ModuleParticles.h"
#include "BaseTransformParticleNode.h"

RotationParticleNode::RotationParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_ROTATION, position, size)
{
}

void RotationParticleNode::Init()
{
	particle->baseTransform->rotation = Quat::FromEulerXYZ(0.0f, 0.0f, DEGTORAD*angle);
}

void RotationParticleNode::Execute(float dt)
{
	particle->baseTransform->rotation = Quat::FromEulerXYZ(0.0f, 0.0f, particle->baseTransform->rotation.ToEulerXYZ().z+(DEGTORAD*angle*dt));
}

EntityData* RotationParticleNode::Copy(Particle* particle) const
{
	RotationParticleNode* ret = new RotationParticleNode(particle, name.c_str(), position, size);

	ret->update = update;
	ret->angle = angle;

	return ret;
}

void RotationParticleNode::DisplayConfig()
{
	ImGui::InputFloat("Angle", &angle);
}

void RotationParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addBool("update", update);
	node->addFloat("angle", angle);
}

void RotationParticleNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	update = nodeDef->getBool("update");
	angle = nodeDef->getFloat("angle");
}
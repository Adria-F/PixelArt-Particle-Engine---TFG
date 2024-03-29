#include "Application.h"
#include "SpeedParticleNode.h"

#include "ModuleParticles.h"
#include "BaseMovementParticleNode.h"
#include "ModuleGUI.h"

SpeedParticleNode::SpeedParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_SPEED, position, size)
{
}

void SpeedParticleNode::Init()
{
	particle->baseMovement->speed = speed;
}

void SpeedParticleNode::Execute(float dt)
{
	BROFILER_CATEGORY("SpeedParticleNode", Profiler::Color::DarkGoldenRod);

	particle->baseMovement->speed += speed*dt;
}

EntityData* SpeedParticleNode::Copy(Particle* particle) const
{
	SpeedParticleNode* ret = new SpeedParticleNode(particle, name.c_str(), position, size);

	ret->update = update;
	ret->speed = speed;

	return ret;
}

void SpeedParticleNode::DisplayConfig()
{
	App->gui->DrawInputFloat("Speed", &speed);
}

void SpeedParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addBool("update", update);
	node->addFloat("speed", speed);
}

void SpeedParticleNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	update = nodeDef->getBool("update");
	speed = nodeDef->getFloat("speed");
}

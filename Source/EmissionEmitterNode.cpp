#include "EmissionEmitterNode.h"

#include "ModuleParticles.h"

EmissionEmitterNode::EmissionEmitterNode(ParticleEmitter* emitter, const char* name, float2 position, float2 size): EntityData(emitter), CanvasNode(name, position, size)
{
	lastEmit = frequency;
}

void EmissionEmitterNode::Execute(float dt)
{
	lastEmit += dt;

	if (lastEmit >= frequency)
	{
		lastEmit = 0.0f;
		switch (type)
		{
		case EmissionEmitterNode::CONSTANT:
			emitter->SpawnParticle();
			break;
		case EmissionEmitterNode::BURST:
			if (repeatBurst)
			{
				for (int i = 0; i < burst; ++i)
				{
					emitter->SpawnParticle();
				}
			}
			break;
		}
	}
}

void EmissionEmitterNode::Play()
{
	if (type == BURST && !repeatBurst)
	{
		for (int i = 0; i < burst; ++i)
		{
			emitter->SpawnParticle();
		}
	}
}

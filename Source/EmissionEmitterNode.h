#ifndef __EMISSION_EMITTERNODE_H__
#define __EMISSION_EMITTERNODE_H__

#include "EntityData.h"

class EmissionEmitterNode : public EntityData
{
public:

	EmissionEmitterNode(ParticleEmitter* emitter);
	~EmissionEmitterNode() {}

	void Execute(float dt);

	void Play();

public:

	enum {
		CONSTANT,
		BURST
	}type = CONSTANT;

	float frequency = 0.25f;
	float burst = 10.0f;
	bool repeatBurst = false;

private:

	float lastEmit = 0.0f;
};

#endif // !__EMISSION_EMITTERNODE_H__
#ifndef __EMITTERDATA_H__
#define __EMITTERDATA_H__

#include "Globals.h"

class ParticleEmitter;

class EmitterData
{
public:

	EmitterData(ParticleEmitter* emitter) : emitter(emitter)
	{}
	~EmitterData() {}

	virtual void Execute(float dt) {}

	virtual EmitterData* Copy(ParticleEmitter* emitter) const { return new EmitterData(emitter); }

public:

	ParticleEmitter* emitter = nullptr;
};

#endif // !__EMITTERDATA_H__
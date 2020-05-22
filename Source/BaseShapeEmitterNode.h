#ifndef __BASESHAPE_EMITTERNODE_H__
#define __BASESHAPE_EMITTERNODE_H__

#include "EntityData.h"

class BaseShapeEmitterNode : public EntityData
{
public:

	BaseShapeEmitterNode(ParticleEmitter* emitter): EntityData(emitter)
	{}
	~BaseShapeEmitterNode() {}

	vec GetDirection() { return vec(0.0f, 1.0f, 0.0f); }
};

#endif // !__BASESHAPE_EMITTERNODE_H__
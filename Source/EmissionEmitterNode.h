#ifndef __EMISSION_EMITTERNODE_H__
#define __EMISSION_EMITTERNODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class EmissionEmitterNode : public EntityData, public CanvasNode
{
public:

	EmissionEmitterNode(ParticleEmitter* emitter, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { STANDARD_NODE_WIDTH, STANDARD_NODE_HEIGHT });
	~EmissionEmitterNode() {}

	void Execute(float dt);

	void Play();

	bool OnConnection(CanvasNode* node);
	void OnDisconnection(NodeConnection* connection);

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
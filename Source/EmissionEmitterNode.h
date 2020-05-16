#ifndef __EMISSION_EMITTERNODE_H__
#define __EMISSION_EMITTERNODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

class EmissionEmitterNode : public EntityData, public CanvasNode
{
public:

	EmissionEmitterNode(ParticleEmitter* emitter, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~EmissionEmitterNode() {}

	void Execute(float dt);

	void Play();

	void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

public:

	enum {
		CONSTANT,
		BURST
	}type = BURST;

	float frequency = 0.25f;
	float burst = 10.0f;
	bool repeatBurst = false;

private:

	float lastEmit = 0.0f;
};

#endif // !__EMISSION_EMITTERNODE_H__
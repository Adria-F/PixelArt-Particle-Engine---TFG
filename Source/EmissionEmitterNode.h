#ifndef __EMISSION_EMITTERNODE_H__
#define __EMISSION_EMITTERNODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

struct Burst
{
	float start = 0.0f;
	int amount = 10;
	float repeat = 0.0f;
	float lastEmit = 0.0f;
};

class EmissionEmitterNode : public EntityData, public CanvasNode
{
public:

	EmissionEmitterNode(ParticleEmitter* emitter, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~EmissionEmitterNode();

	void Execute(float dt);

	void Stop();

	void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadExtraInfo(JSON_Value* nodeDef);

public:

	float frequency = 0.25f;
	float duration = 5.0f;
	bool loop = true;

	std::list<Burst*> bursts;

private:

	float time = 0.0f;
	float lastEmit = 0.0f;
};

#endif // !__EMISSION_EMITTERNODE_H__
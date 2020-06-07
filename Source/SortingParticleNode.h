#ifndef __SORTING_PARTICLENODE_H__
#define __SORTING_PARTICLENODE_H__

#include "EntityData.h"
#include "CanvasNode.h"

enum sortingType
{
	SORT_OLDER,
	SORT_YOUNGER
};

class SortingParticleNode : public EntityData, public CanvasNode
{
public:

	SortingParticleNode(Particle* particle, const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT });
	~SortingParticleNode() {}

	EntityData* Copy(Particle* particle) const;

	void DrawExtraInfo(float2 offset, int zoom);
	void DisplayConfig();

	void SaveExtraInfo(JSON_Value* node);
	void LoadeExtraInfo(JSON_Value* nodeDef);

public:

	sortingType type = SORT_OLDER;
	int priority = 0;
};

#endif // !__SORTING_PARTICLENODE_H__
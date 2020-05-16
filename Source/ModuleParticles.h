#ifndef __MODULEPARTICLES_H__
#define __MODULEPARTICLES_H__

#include "Module.h"
#include "Globals.h"
#include <list>
#include <unordered_map>

#include "NodeGroup.h"

class EntityData;

// Particle ------------------
class Particle;

class BaseTransformParticleNode;
class BaseMovementParticleNode;
class BaseColorParticleNode;

class ColorParticleNode;
class SpeedParticleNode;
class MakeGlobalParticleNode;
class DeathInstantiationParticleNode;

//Emitter ---------------------
class ParticleEmitter;

class BaseTransformEmitterNode;

class EmissionEmitterNode;
class TransformEmitterNode;
class InputParticleEmitterNode;

class Particle: public NodeGroup
{
public:

	Particle(const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT }, bool empty = false);
	Particle(ParticleEmitter* emitter, Particle* templateParticle);
	~Particle();

	void Update(float dt);
	void Draw();

	void SetRandomLifeTime(bool random);
	float GetLifePercent() const;

	//Canvas
	void OnNodeAdded(CanvasNode* node);
	void OnNodeRemoved(CanvasNode* node);

	bool OnConnection(NodeConnection* connection);
	void OnDisconnection(NodeConnection* connection);

	void DisplayConfig();

public:

	union
	{
		struct
		{
			ColorParticleNode* color;
			SpeedParticleNode* speed;
			MakeGlobalParticleNode* makeGlobal;
			DeathInstantiationParticleNode* deathInstantiation;

			//Base nodes last as other nodes can modify their information
			BaseTransformParticleNode* baseTransform;
			BaseMovementParticleNode* baseMovement;		
			BaseColorParticleNode* baseColor;		
		};
		EntityData* data[MAX_ENTITY_DATA];
	};

	ParticleEmitter* emitter = nullptr;

	float lifeTime = 5.0f;

	float randLifeTime1 = 3.0f;
	float randLifeTime2 = 5.0f;

	bool toDestroy = false;

private:

	float timeAlive = 0.0f;
	bool randomizeLifeTime = false;
};

class ParticleEmitter: public NodeGroup
{
public:

	ParticleEmitter(const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT }, bool empty = false);
	~ParticleEmitter();

	void Play();
	void Pause();
	void Stop();

	void Update(float dt);
	void UpdateParticles(float dt);
	void DrawParticles();

	void SpawnParticle();
	Particle* SpawnParticle(Particle* particle);
	vec randomDirectionInCone(float radius, float height) const;

	void SetTemplate(Particle* templateParticle);
	Particle* GetTemplate() const;
	int GetParticleCount() const;

	//Canvas
	void OnNodeAdded(CanvasNode* node);
	void OnNodeRemoved(CanvasNode* node);

public:

	union
	{
		struct
		{
			EmissionEmitterNode* emission;
			TransformEmitterNode* transform;
			InputParticleEmitterNode* inputParticle;

			//Base nodes last as other nodes can modify their information
			BaseTransformEmitterNode* baseTransform;
		};
		EntityData* data[MAX_ENTITY_DATA];
	};

	float frequency = 0.0f;

private:

	Particle* templateParticle = nullptr;
	std::list<Particle*> particles;

	float lastEmit = 0.0f;
	bool playing = true;
	bool restarted = false;
};

class ModuleParticles : public Module
{
public:
	ModuleParticles(bool start_enabled = true);
	~ModuleParticles();

	bool Start();
	update_state Update(float dt);
	bool CleanUp();

	void Play();
	void Pause();
	void Stop();
	void DrawParticles();

	void AddEmitter(ParticleEmitter* emitter);
	void RemoveEmitter(ParticleEmitter* emitter);
	ParticleEmitter* GetEmitter(int index) const;
	void ClearAll();

private:

	uint rectangleVAO;

	std::list<ParticleEmitter*> emitters;
};

#endif // !__MODULEPARTICLES_H__
#ifndef __MODULEPARTICLES_H__
#define __MODULEPARTICLES_H__

#include "Module.h"
#include "Globals.h"
#include <list>
#include <unordered_map>

#include "NodeGroup.h"

#define MAX_PARTICLES 1000

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
class SpriteParticleNode;
class LifetimeParticleNode;
class TransformParticleNode;
class BlendModeParticleNode;
class SortingParticleNode;

//Emitter ---------------------
class ParticleEmitter;

class BaseTransformEmitterNode;
class BaseShapeEmitterNode;

class EmissionEmitterNode;
class ShapeEmitterNode;
class TransformEmitterNode;
class InputParticleEmitterNode;

class Particle: public NodeGroup
{
public:

	Particle(const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT }, bool empty = false);
	Particle(ParticleEmitter* emitter, Particle* templateParticle);
	Particle();
	~Particle();

	void Initialize(Particle* templateParticle);

	void Update(float dt);
	void Draw();

	float GetLifePercent() const;

	//Canvas
	void OnNodeAdded(CanvasNode* node, bool update);
	void OnNodeRemoved(CanvasNode* node);

	bool OnConnection(NodeConnection* connection);
	void OnDisconnection(NodeConnection* connection);

public:

	union
	{
		struct
		{
			ColorParticleNode* color;
			SpeedParticleNode* speedInit;
			SpeedParticleNode* speedUpdate;
			MakeGlobalParticleNode* makeGlobal;
			DeathInstantiationParticleNode* deathInstantiation;
			SpriteParticleNode* sprite;
			LifetimeParticleNode* lifetimeNode;
			TransformParticleNode* transformInit;
			TransformParticleNode* transformUpdate;
			BlendModeParticleNode* blendMode;
			SortingParticleNode* sorting;

			//Base nodes last as other nodes can modify their information
			BaseTransformParticleNode* baseTransform;
			BaseMovementParticleNode* baseMovement;		
			BaseColorParticleNode* baseColor;		
		};
		EntityData* data[MAX_ENTITY_DATA];
	};

	ParticleEmitter* emitter = nullptr;

	float lifeTime = 5.0f;

	uint whiteSprite = 0;

	//Particle
	bool toDestroy = false;
	bool alive = false;
	float timeAlive = 0.0f;
};

class ParticleEmitter: public NodeGroup
{
public:

	ParticleEmitter(const char* name, float2 position = { 0.0f,0.0f }, float2 size = { NODE_DEFAULT_WIDTH, NODE_DEFAULT_HEIGHT }, bool empty = false);
	~ParticleEmitter();

	void Play();
	void Pause();
	void Stop();
	void Finish();

	bool Playing() const;
	bool Stopped() const;
	bool Finished() const;

	void Update(float dt);
	void UpdateParticles(float dt);
	void SendParticlesToBuffer();
	void DrawParticles();

	void SpawnParticle();
	Particle* SpawnParticle(Particle* particle);

	void SetTemplate(Particle* templateParticle);
	Particle* GetTemplate() const;
	int GetParticleCount() const;

	//Canvas
	void OnNodeAdded(CanvasNode* node, bool update);
	void OnNodeRemoved(CanvasNode* node);

public:

	union
	{
		struct
		{
			EmissionEmitterNode* emission;
			ShapeEmitterNode* shape;
			TransformEmitterNode* transform;
			InputParticleEmitterNode* inputParticle;

			//Base nodes last as other nodes can modify their information
			BaseTransformEmitterNode* baseTransform;
			BaseShapeEmitterNode* baseShape;
		};
		EntityData* data[MAX_ENTITY_DATA];
	};

	float frequency = 0.5f;
	float timeAlive = 0.0f;

private:

	Particle* templateParticle = nullptr;
	std::list<Particle*> particles;

	float lastEmit = 0.0f;
	bool playing = true;
	bool restarted = false;
	bool finished = false;
};

class ModuleParticles : public Module
{
public:
	ModuleParticles(bool start_enabled = true);
	~ModuleParticles();

	bool Start();
	update_state Update(float dt);
	bool CleanUp();

	void SaveSettings(JSON_Value* settings);
	void LoadSettings(JSON_Value* settings);

	void Play();
	void Pause();
	void Stop();
	void SendParticlesToBuffer();
	void DrawParticles();

	bool IsPlaying();
	bool IsFinished();

	Particle* GetParticle();
	int GetParticleCount() const;

	void AddEmitter(ParticleEmitter* emitter);
	void RemoveEmitter(ParticleEmitter* emitter);
	ParticleEmitter* GetEmitter(int index) const;
	void ClearAll();

	int GetParticlePoolSize() const;
	void ResizeParticlePool(int newSize);

private:

	uint rectangleVAO;

	std::list<ParticleEmitter*> emitters;
	std::vector<Particle> particles;

	int lastSpawnedParticle = 0;
};

#endif // !__MODULEPARTICLES_H__
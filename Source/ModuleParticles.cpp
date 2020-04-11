#include "Application.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "Shader.h"
#include "ModuleCamera.h"
#include "ModuleNodeCanvas.h"

#include "EntityData.h"

//Include all particle data nodes
#include "BaseTransformParticleNode.h"
#include "BaseMovementParticleNode.h"
#include "BaseColorParticleNode.h"

#include "ColorParticleNode.h"
#include "SpeedParticleNode.h"
#include "MakeGlobalParticleNode.h"

//Include all emitter data nodes
#include "BaseTransformEmitterNode.h"

#include "EmissionEmitterNode.h"

ModuleParticles::ModuleParticles(bool start_enabled) : Module(start_enabled)
{
}

ModuleParticles::~ModuleParticles()
{
}

bool ModuleParticles::Start()
{
	//Create rectangle VAO
	float vertices[] = {
			 0.5f,  0.5f, 0.0f,  // top right
			 0.5f, -0.5f, 0.0f,  // bottom right
			-0.5f, -0.5f, 0.0f,  // bottom left
			-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {
		0, 3, 1,  // first Triangle
		1, 3, 2   // second Triangle
	};

	rectangleVAO = App->render->generateVAO(sizeof(vertices), &vertices[0], sizeof(indices), &indices[0]);

	//As it is the only shape we are drawing, we can left it binded
	glBindVertexArray(rectangleVAO);

	return true;
}

update_state ModuleParticles::Update(float dt)
{
	for (std::list<ParticleEmitter*>::iterator it_e = emitters.begin(); it_e != emitters.end(); ++it_e)
	{
		(*it_e)->Update(dt);
		(*it_e)->UpdateParticles(dt);
	}

	return update_state::UPDATE_CONTINUE;
}

bool ModuleParticles::CleanUp()
{
	//TODO: Remove VAO

	return true;
}

void ModuleParticles::Play()
{
	for (std::list<ParticleEmitter*>::iterator it_e = emitters.begin(); it_e != emitters.end(); ++it_e)
	{
		(*it_e)->Play();
	}
}

void ModuleParticles::Pause()
{
	for (std::list<ParticleEmitter*>::iterator it_e = emitters.begin(); it_e != emitters.end(); ++it_e)
	{
		(*it_e)->Pause();
	}
}

void ModuleParticles::Stop()
{
	for (std::list<ParticleEmitter*>::iterator it_e = emitters.begin(); it_e != emitters.end(); ++it_e)
	{
		(*it_e)->Stop();
	}
}

void ModuleParticles::DrawParticles()
{
	for (std::list<ParticleEmitter*>::iterator it_e = emitters.begin(); it_e != emitters.end(); ++it_e)
	{
		(*it_e)->DrawParticles();
	}
}

void ModuleParticles::AddEmitter(ParticleEmitter* emitter)
{
	emitters.push_back(emitter);
}

void ModuleParticles::RemoveEmitter(ParticleEmitter* emitter)
{
	emitters.remove(emitter);
}

ParticleEmitter* ModuleParticles::GetEmitter(int index) const
{
	if (index < emitters.size())
	{
		return std::next(emitters.front(), index);
	}

	return nullptr;
}

// ---------------------- PARTICLE EMITTER --------------------------

ParticleEmitter::ParticleEmitter(const char* name, float2 position, float2 size): CanvasNode(name, EMITTER, position, size)
{
	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		data[i] = nullptr;
	}

	baseTransform = new BaseTransformEmitterNode(this);
	lastEmit = frequency;

	NodeConnection* particleIn = new NodeConnection(this, NODE_INPUT, { size.x - GRAPH_NODE_WINDOW_PADDING * 0.25f, size.y/2.0f }, TRIANGLE, ImGuiDir_Left);
	connections.push_back(particleIn);

	NodeConnection* emitterIn = new NodeConnection(this, NODE_INPUT, { size.x / 2.0f, size.y }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(emitterIn);
}

ParticleEmitter::~ParticleEmitter()
{
	for (std::list<Particle*>::iterator it_p = particles.begin(); it_p != particles.end(); ++it_p)
	{
		RELEASE((*it_p));
	}
	particles.clear();
}

void ParticleEmitter::Play()
{
	if (restarted)
	{
		restarted = false;

		//Grant spawn first particle at frame 0
		lastEmit = frequency;
	}
	
	if ((playing || restarted) && emission != nullptr)
	{
		emission->Play();
	}

	playing = true;
}

void ParticleEmitter::Pause()
{
	playing = false;
}

void ParticleEmitter::Stop()
{
	playing = false;
	restarted = true;

	//Delete all particles
	for (std::list<Particle*>::iterator it_p = particles.begin(); it_p != particles.end(); ++it_p)
	{
		RELEASE((*it_p));
	}
	particles.clear();
}

void ParticleEmitter::Update(float dt)
{
	if (playing && templateParticle != nullptr)
	{
		if (emission != nullptr)
		{
			emission->Execute(dt);
		}
		else
		{
			lastEmit += dt;

			if (lastEmit >= frequency)
			{
				lastEmit = 0.0f;
				SpawnParticle();
			}
		}
	}
}

void ParticleEmitter::UpdateParticles(float dt)
{
	for (std::list<Particle*>::iterator it_p = particles.begin(); it_p != particles.end(); ++it_p)
	{
		(*it_p)->baseTransform->LookCamera();
		if (playing)
			(*it_p)->Update(dt);
	}

	//Check particles to destroy before drawing
	std::list<Particle*>::iterator it_p = particles.begin();
	while(it_p != particles.end())
	{
		if ((*it_p)->toDestroy)
		{
			RELEASE((*it_p));
			it_p = particles.erase(it_p);
		}
		else
			++it_p;
	}
}

void ParticleEmitter::DrawParticles()
{
	for (std::list<Particle*>::iterator it_p = particles.begin(); it_p != particles.end(); ++it_p)
	{
		(*it_p)->Draw();
	}
}

void ParticleEmitter::SpawnParticle()
{
	Particle* part = new Particle(this, templateParticle);
	part->baseMovement->direction = randomDirectionInCone(3.0f, 5.0f); //TODO: Get direction from shape node
	particles.push_back(part);
}

vec ParticleEmitter::randomDirectionInCone(float radius, float height) const
{
	float angle = 2.0*PI * GET_RANDOM();
	float x = Cos(angle)* radius*GET_RANDOM();
	float z = Sin(angle)* radius*GET_RANDOM();

	vec point = vec(x, height, z);

	return point.Normalized();
}

Particle* ParticleEmitter::GetTemplate() const
{
	return templateParticle;
}

int ParticleEmitter::GetParticleCount() const
{
	return particles.size();
}

bool ParticleEmitter::OnConnection(CanvasNode* node)
{
	bool ret = false;

	if (node->type == PARTICLE)
	{
		templateParticle = (Particle*)node;
		Play(); //TMP if scene is playing
		ret = true;
	}

	switch (node->type)
	{
	case PARTICLE:
		templateParticle = (Particle*)node;
		Play(); //TMP if scene is playing
		ret = true;
		break;
	case EMITTER_EMISSION:
		emission = (EmissionEmitterNode*)node;
		ret = true;
		break;
	}

	return ret;
}

void ParticleEmitter::OnDisconnection(NodeConnection* connection)
{
	switch (connection->node->type)
	{
	case PARTICLE:
		Stop();
		templateParticle = nullptr;
		break;
	case EMITTER_EMISSION:
		emission = nullptr;
		break;
	}
}

// ----------------------------- PARTICLE ----------------------------------

Particle::Particle(const char* name, float2 position, float2 size): CanvasNode(name, PARTICLE, position, size)
{
	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		data[i] = nullptr;
	}

	baseTransform = new BaseTransformParticleNode(this);
	baseMovement = new BaseMovementParticleNode(this);
	baseColor = new BaseColorParticleNode(this);

	particleOut = new NodeConnection(this, NODE_OUTPUT, { 0.0f, size.y / 2.0f }, TRIANGLE, ImGuiDir_Left);
	connections.push_back(particleOut);

	NodeConnection* particleIn = new NodeConnection(this, NODE_INPUT, { size.x / 2.0f, size.y }, TRIANGLE, ImGuiDir_Up);
	connections.push_back(particleIn);
}

Particle::Particle(ParticleEmitter* emitter, Particle* templateParticle): emitter(emitter)
{
	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		data[i] = nullptr;
		if (templateParticle->data[i] != nullptr)
		{
			data[i] = templateParticle->data[i]->Copy(this);
		}
	}

	lifeTime = templateParticle->lifeTime;
	randLifeTime1 = templateParticle->randLifeTime1;
	randLifeTime2 = templateParticle->randLifeTime2;
	randomizeLifeTime = templateParticle->randomizeLifeTime;

	if (randomizeLifeTime)
	{
		lifeTime = Lerp(randLifeTime1, randLifeTime2, GET_RANDOM());
	}
}

Particle::~Particle()
{
	if (particleOut == nullptr) //If it has node connections means that it's managed from canvas, so data will be deleted from there as nodes (template particle)
	{
		for (int i = 0; i < MAX_ENTITY_DATA; ++i)
		{
			RELEASE(data[i]);
		}
	}
}

void Particle::Update(float dt)
{
	timeAlive += dt;

	if (timeAlive >= lifeTime)
		toDestroy = true;

	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		if (data[i] != nullptr)
			data[i]->Execute(dt);
	}
}

void Particle::Draw()
{
	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		if (data[i] != nullptr)
			data[i]->PrepareRender();
	}

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Particle::SetRandomLifeTime(bool random)
{
	randomizeLifeTime = random;
}

float Particle::GetLifePercent() const
{
	return timeAlive/lifeTime;
}

bool Particle::OnConnection(CanvasNode* node)
{
	bool ret = false;

	switch (node->type)
	{
	case PARTICLE_COLOR:
		color = (ColorParticleNode*)node;
		ret = true;
		break;
	case PARTICLE_SPEED:
		speed = (SpeedParticleNode*)node;
		ret = true;
		break;
	case PARTICLE_MAKEGLOBAL:
		makeGlobal = (MakeGlobalParticleNode*)node;
		ret = true;
		break;
	case EMITTER:
		emitter = (ParticleEmitter*)node;
		ret = true;
		break;
	}

	return ret;
}

void Particle::OnDisconnection(NodeConnection* connection)
{
	switch (connection->node->type)
	{
	case PARTICLE_COLOR:
		color = nullptr;
		break;
	case PARTICLE_SPEED:
		speed = nullptr;
		break;
	case PARTICLE_MAKEGLOBAL:
		makeGlobal = nullptr;
		break;
	case EMITTER:
		emitter = nullptr;
		break;
	}
}

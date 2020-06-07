#include "Application.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "Shader.h"
#include "ModuleCamera.h"
#include "ModuleNodeCanvas.h"
#include "ModuleGUI.h"
#include "ModuleTextures.h"

#include "EntityData.h"

//Include all particle data nodes
#include "BaseTransformParticleNode.h"
#include "BaseMovementParticleNode.h"
#include "BaseColorParticleNode.h"

#include "ColorParticleNode.h"
#include "SpeedParticleNode.h"
#include "MakeGlobalParticleNode.h"
#include "DeathInstantiationParticleNode.h"
#include "SpriteParticleNode.h"
#include "LifetimeParticleNode.h"
#include "TransformParticleNode.h"
#include "BlendModeParticleNode.h"
#include "SortingParticleNode.h"

//Include all emitter data nodes
#include "BaseTransformEmitterNode.h"
#include "BaseShapeEmitterNode.h"

#include "EmissionEmitterNode.h"
#include "ShapeEmitterNode.h"
#include "TransformEmitterNode.h"
#include "InputParticleEmitterNode.h"

ModuleParticles::ModuleParticles(bool start_enabled) : Module(start_enabled)
{
	particles = std::vector<Particle>(MAX_PARTICLES);
}

ModuleParticles::~ModuleParticles()
{
}

bool ModuleParticles::Start()
{
	//Create rectangle VAO
	float vertices[] = {
		// positions       
		 0.5f,  0.5f, 0.0f, // top right
		 0.5f, -0.5f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f // top left 
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
	BROFILER_CATEGORY("ModuleParticles Update", Profiler::Color::Orange);

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

	emitters.clear();

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

void ModuleParticles::SendParticlesToBuffer()
{
	BROFILER_CATEGORY("Fill Render Buffer", Profiler::Color::LightSalmon);

	for (std::list<ParticleEmitter*>::iterator it_e = emitters.begin(); it_e != emitters.end(); ++it_e)
	{
		(*it_e)->SendParticlesToBuffer();
	}
}

void ModuleParticles::DrawParticles()
{
	BROFILER_CATEGORY("Draw Particles", Profiler::Color::Orange);

	for (std::list<ParticleEmitter*>::iterator it_e = emitters.begin(); it_e != emitters.end(); ++it_e)
	{
		(*it_e)->DrawParticles();
	}
}

Particle* ModuleParticles::GetParticle()
{
	Particle* ret = nullptr;

	for (int i = lastSpawnedParticle+1; i < MAX_PARTICLES; ++i)
	{
		if (!particles[i].alive)
		{
			ret = &particles[i];
			lastSpawnedParticle = i;
			break;
		}
	}
	if (ret == nullptr)
	{
		for (int i = 0; i < lastSpawnedParticle+1; ++i)
		{
			if (!particles[i].alive)
			{
				ret = &particles[i];
				lastSpawnedParticle = i;
				break;
			}
		}
	}

	return ret;
}

int ModuleParticles::GetParticleCount() const
{
	int ret = 0;

	for (std::list<ParticleEmitter*>::const_iterator it_e = emitters.begin(); it_e != emitters.end(); ++it_e)
	{
		ret += (*it_e)->GetParticleCount();
	}

	return ret;
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

void ModuleParticles::ClearAll()
{
	emitters.clear();
}

// ---------------------- PARTICLE EMITTER --------------------------

ParticleEmitter::ParticleEmitter(const char* name, float2 position, float2 size, bool empty): NodeGroup(name, position, EMITTER)
{
	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		data[i] = nullptr;
	}

	baseTransform = new BaseTransformEmitterNode(this);
	baseShape = new BaseShapeEmitterNode(this);
	lastEmit = frequency;

	//Node boxes
	if (!empty)
	{
		AddNodeBox("Emitter SetUp", EMITTER_NODE_BOX_INIT);
		AddNodeBox("Emitting Particle", EMITTER_NODE_BOX_INPUT);
		AddNodeBox("Emitter Update", EMITTER_NODE_BOX_UPDATE);
	}
}

ParticleEmitter::~ParticleEmitter()
{
	for (std::list<Particle*>::iterator it_p = particles.begin(); it_p != particles.end(); ++it_p)
	{
		(*it_p)->alive = false;
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
	timeAlive = 0.0f;

	if (emission != nullptr)
		emission->Stop();

	//Delete all particles
	for (std::list<Particle*>::iterator it_p = particles.begin(); it_p != particles.end(); ++it_p)
	{
		(*it_p)->alive = false;
	}
	particles.clear();
}

void ParticleEmitter::Update(float dt)
{
	BROFILER_CATEGORY("Update Emitter", Profiler::Color::LightGreen);

	if (inputParticle != nullptr)
		inputParticle->Execute(dt);

	if (playing)
	{
		timeAlive += dt;

		for (int i = 0; i < MAX_ENTITY_DATA; ++i)
		{
			if (data[i] != nullptr && data[i]->update)
				data[i]->Execute(dt);
		}

		if (emission == nullptr)
		{
			lastEmit += dt;

			while (lastEmit >= frequency)
			{
				lastEmit -= frequency;
				SpawnParticle();
			}
		}
	}
}

void ParticleEmitter::UpdateParticles(float dt)
{
	BROFILER_CATEGORY("Update Particles", Profiler::Color::DarkRed);

	for (std::list<Particle*>::iterator it_p = particles.begin(); it_p != particles.end(); ++it_p)
	{
		if ((*it_p)->baseTransform->billboard && App->camera->type == CAMERA_3D)
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
			(*it_p)->alive = false; //Set particle as dead (not using)
			it_p = particles.erase(it_p);
		}
		else
			++it_p;
	}
}

void ParticleEmitter::SendParticlesToBuffer()
{
	for (std::list<Particle*>::iterator it_p = particles.begin(); it_p != particles.end(); ++it_p)
	{
		if (App->camera->type == CAMERA_3D)
			App->render->proximityBuffer.push((*it_p));
		else
		{
			if ((*it_p)->sorting == nullptr || (*it_p)->sorting->type == SORT_OLDER)
			{
				App->render->layers[((*it_p)->sorting)? (*it_p)->sorting->priority : 0].olderBuffer.push((*it_p));
			}
			else
			{
				App->render->layers[(*it_p)->sorting->priority].youngerBuffer.push((*it_p));
			}
		}
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
	if (templateParticle != nullptr)
	{
		Particle* part = App->particles->GetParticle();
		if (part != nullptr)
		{
			part->Initialize(templateParticle);
			vec direction = (shape == nullptr) ? baseShape->GetDirection() : shape->GetDirection();
			part->baseMovement->direction = direction.Normalized();
			if (part->transformInit != nullptr)
			{
				part->transformInit->SetSpawnPoint(direction);
				if (part->transformInit->faceDirection)
					part->transformInit->Init();
			}
			particles.push_back(part);
		}
	}
}

Particle* ParticleEmitter::SpawnParticle(Particle* particle) //Used by Death Instantiate
{
	Particle* ret = App->particles->GetParticle();
	if (ret != nullptr)
	{
		ret->Initialize(templateParticle);
		particles.push_back(ret);
	}
	return ret;
}

void ParticleEmitter::SetTemplate(Particle* templateParticle)
{
	if (templateParticle != nullptr)
		Play(); //TMP only if scene is playing
	else
		Stop();

	this->templateParticle = templateParticle;
}

Particle* ParticleEmitter::GetTemplate() const
{
	return templateParticle;
}

int ParticleEmitter::GetParticleCount() const
{
	return particles.size();
}

void ParticleEmitter::OnNodeAdded(CanvasNode* node, bool update)
{
	switch (node->type)
	{
	case EMITTER_EMISSION:
		emission = (EmissionEmitterNode*)node;
		emission->emitter = this;
		break;
	case EMITTER_SHAPE:
		shape = (ShapeEmitterNode*)node;
		shape->emitter = this;
		break;
	case EMITTER_TRANSFORM:
		transform = (TransformEmitterNode*)node;
		transform->emitter = this;
		break;
	case EMITTER_INPUTPARTICLE:
		inputParticle = (InputParticleEmitterNode*)node;
		inputParticle->emitter = this;
		break;
	}
}

void ParticleEmitter::OnNodeRemoved(CanvasNode* node)
{
	switch (node->type)
	{
	case EMITTER_EMISSION:
		emission = nullptr;
		break;
	case EMITTER_SHAPE:
		shape = nullptr;
		break;
	case EMITTER_TRANSFORM:
		transform = nullptr;
		break;
	case EMITTER_INPUTPARTICLE:
		Stop();
		inputParticle = nullptr;
		break;
	}
}

// ----------------------------- PARTICLE ----------------------------------

Particle::Particle(const char* name, float2 position, float2 size, bool empty): NodeGroup(name, position, PARTICLE)
{
	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		data[i] = nullptr;
	}

	baseTransform = new BaseTransformParticleNode(this);
	baseMovement = new BaseMovementParticleNode(this);
	baseColor = new BaseColorParticleNode(this);

	if (!empty)
	{
		AddNodeBox("Particle Set Up", PARTICLE_NODE_BOX_INIT);
		AddNodeBox("Particle Update", PARTICLE_NODE_BOX_UPDATE);
		AddNodeBox("Particle Render", PARTICLE_NODE_BOX_RENDER);
	}

	NodeConnection* particleOut = new NodeConnection(this, NODE_OUTPUT, { -CONNECTIONTRIANGLE_SIZE*0.5f, 25.0f }, TRIANGLE, ImGuiDir_Left);
	connections.push_back(particleOut);
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

	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		if (data[i] != nullptr && !data[i]->update)
			data[i]->Init(); //Initialize when this node does not update (must be called only once)
	}

	if (lifetimeNode == nullptr)
		lifeTime = templateParticle->lifeTime;

	whiteSprite = App->textures->UseWhiteTexture();
}

Particle::Particle()
{
	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		data[i] = nullptr;
	}
}

Particle::~Particle()
{
	if (boxes.size() == 0) //If it has NodeBoxes means that it's managed from canvas, so data will be deleted from there as nodes (template particle)
	{
		for (int i = 0; i < MAX_ENTITY_DATA; ++i)
		{
			RELEASE(data[i]);
		}
	}
}

void Particle::Initialize(Particle* templateParticle)
{
	toDestroy = false;
	alive = true;
	timeAlive = 0.0f;

	emitter = templateParticle->emitter;

	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		RELEASE(data[i]);
		if (templateParticle->data[i] != nullptr)
		{
			data[i] = templateParticle->data[i]->Copy(this);
		}
	}

	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		if (data[i] != nullptr && !data[i]->update)
			data[i]->Init(); //Initialize when this node does not update (must be called only once)
	}

	if (lifetimeNode == nullptr)
		lifeTime = templateParticle->lifeTime;

	whiteSprite = App->textures->UseWhiteTexture();
}

void Particle::Update(float dt)
{
	timeAlive += dt;

	if (timeAlive >= lifeTime)
		toDestroy = true;

	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		if (data[i] != nullptr && data[i]->update)
			data[i]->Execute(dt);
	}
}

void Particle::Draw()
{
	if (sprite == nullptr)
		App->render->defaultShader->sendTexture("sprite", whiteSprite);
	if (blendMode == nullptr)
		App->render->SetBlendMode(BLEND_ADDITIVE);

	BROFILER_CATEGORY("Prepare Render", Profiler::Color::Brown);

	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		if (data[i] != nullptr)
			data[i]->PrepareRender();
	}

	BROFILER_CATEGORY("Draw Elements", Profiler::Color::Brown);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

float Particle::GetLifePercent() const
{
	return timeAlive/lifeTime;
}

void Particle::OnNodeAdded(CanvasNode* node, bool update)
{
	switch (node->type)
	{
	case PARTICLE_COLOR:
		color = (ColorParticleNode*)node;
		color->particle = this;
		break;
	case PARTICLE_SPEED:
		if (update)
		{
			speedUpdate = (SpeedParticleNode*)node;
			speedUpdate->particle = this;
			speedUpdate->update = update;
		}
		else
		{
			speedInit = (SpeedParticleNode*)node;
			speedInit->particle = this;
			speedInit->update = update;
		}
		break;
	case PARTICLE_MAKEGLOBAL:
		makeGlobal = (MakeGlobalParticleNode*)node;
		makeGlobal->particle = this;
		break;
	case PARTICLE_DEATHINSTANTIATION:
		deathInstantiation = (DeathInstantiationParticleNode*)node;
		deathInstantiation->particle = this;
		break;
	case PARTICLE_SPRITE:
		sprite = (SpriteParticleNode*)node;
		sprite->particle = this;
		break;
	case PARTICLE_LIFETIME:
		lifetimeNode = (LifetimeParticleNode*)node;
		lifetimeNode->particle = this;
		break;
	case PARTICLE_TRANSFORM:
		if (update)
		{
			transformUpdate = (TransformParticleNode*)node;
			transformUpdate->particle = this;
			transformUpdate->update = true;
		}
		else
		{
			transformInit = (TransformParticleNode*)node;
			transformInit->particle = this;
			transformInit->update = false;
		}
		break;
	case PARTICLE_BLENDMODE:
		blendMode = (BlendModeParticleNode*)node;
		blendMode->particle = this;
		break;
	case PARTICLE_SORTING:
		sorting = (SortingParticleNode*)node;
		sorting->particle = this;
		break;
	};
}

void Particle::OnNodeRemoved(CanvasNode* node)
{
	switch (node->type)
	{
	case PARTICLE_COLOR:
		color = nullptr;
		break;
	case PARTICLE_SPEED:
		if (speedInit == node)
			speedInit = nullptr;
		else if (speedUpdate == node)
			speedUpdate = nullptr;
		break;
	case PARTICLE_MAKEGLOBAL:
		makeGlobal = nullptr;
		break;
	case PARTICLE_DEATHINSTANTIATION:
		deathInstantiation = nullptr;
		break;
	case PARTICLE_SPRITE:
		sprite = nullptr;
		break;
	case PARTICLE_LIFETIME:
		lifetimeNode = nullptr;
		break;
	case PARTICLE_TRANSFORM:
		if (transformInit == node)
			transformInit = nullptr;
		else if (transformUpdate == node)
			transformUpdate = nullptr;
		break;
	case PARTICLE_BLENDMODE:
		blendMode = nullptr;
		break;
	case PARTICLE_SORTING:
		sorting = nullptr;
		break;
	}
}

bool Particle::OnConnection(NodeConnection* connection)
{
	bool ret = false;

	if (connection->type == NODE_INPUT)
	{
		if (connection->node->type == EMITTER_INPUTPARTICLE)
		{
			emitter = ((InputParticleEmitterNode*)connection->node)->emitter;
			ret = true;
		}
		if (connection->node->type == PARTICLE_DEATHINSTANTIATION)
		{
			ret = true;
		}
	}

	return ret;
}

void Particle::OnDisconnection(NodeConnection* connection)
{
	emitter = nullptr; //Only connection with Input Particle node
}
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

//Include all emitter data nodes
#include "BaseTransformEmitterNode.h"
#include "BaseShapeEmitterNode.h"

#include "EmissionEmitterNode.h"
#include "ShapeEmitterNode.h"
#include "TransformEmitterNode.h"
#include "InputParticleEmitterNode.h"

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
	for (std::list<ParticleEmitter*>::iterator it_e = emitters.begin(); it_e != emitters.end(); ++it_e)
	{
		(*it_e)->SendParticlesToBuffer();
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
		RELEASE((*it_p));
	}
	particles.clear();
}

void ParticleEmitter::Update(float dt)
{
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
			RELEASE((*it_p));
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
		App->render->renderBuffer.push((*it_p));
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
		Particle* part = new Particle(this, templateParticle);
		vec direction = (shape == nullptr) ? baseShape->GetDirection() : shape->GetDirection();
		part->baseMovement->direction = direction;
		particles.push_back(part);
	}
}

Particle* ParticleEmitter::SpawnParticle(Particle* particle)
{
	Particle* ret = new Particle(this, particle);
	particles.push_back(ret);
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

	for (int i = 0; i < MAX_ENTITY_DATA; ++i)
	{
		if (data[i] != nullptr)
			data[i]->PrepareRender();
	}

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
			rotationUpdate = (TransformParticleNode*)node;
			rotationUpdate->particle = this;
			rotationUpdate->update = true;
		}
		else
		{
			rotationInit = (TransformParticleNode*)node;
			rotationInit->particle = this;
			rotationInit->update = false;
		}
		break;
	case PARTICLE_BLENDMODE:
		blendMode = (BlendModeParticleNode*)node;
		blendMode->particle = this;
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
		if (rotationInit == node)
			rotationInit = nullptr;
		else if (rotationUpdate == node)
			rotationUpdate = nullptr;
		break;
	case PARTICLE_BLENDMODE:
		blendMode = nullptr;
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
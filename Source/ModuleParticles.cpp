#include "Application.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "Shader.h"
#include "ModuleCamera.h"
#include "ModuleNodeCanvas.h"
#include "ModuleGUI.h"

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

void ParticleEmitter::DisplayConfig()
{
	ImGui::Text("Emitter");
	ImGui::SameLine();

	//Transform
	ImGui::Text("Transform:");
	bool changed = false;
	///Position
	ImGui::Dummy({ 25.0f, 0.0f }); ImGui::SameLine();
	ImGui::Text("Position"); ImGui::SameLine();
	if (App->gui->DrawInputFloat("X", "##posX", &baseTransform->position.x, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Y", "##posY", &baseTransform->position.y, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Z", "##posZ", &baseTransform->position.z, 0.1f, true))
		changed = true;
	///Rotation
	ImGui::Dummy({ 25.0f, 0.0f }); ImGui::SameLine();
	ImGui::Text("Rotation"); ImGui::SameLine();
	if (App->gui->DrawInputFloat("X", "##rotX", &baseTransform->rotationEuler.x, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Y", "##rotY", &baseTransform->rotationEuler.y, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Z", "##rotZ", &baseTransform->rotationEuler.z, 0.1f, true))
		changed = true;
	///Scale
	ImGui::Dummy({ 25.0f, 0.0f }); ImGui::SameLine();
	ImGui::Text("Scale"); ImGui::SameLine();
	if (App->gui->DrawInputFloat("X", "##scaleX", &baseTransform->scale.x, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Y", "##scaleY", &baseTransform->scale.y, 0.1f, true))
		changed = true;
	ImGui::SameLine();
	if (App->gui->DrawInputFloat("Z", "##scaleZ", &baseTransform->scale.z, 0.1f, true))
		changed = true;
	ImGui::Separator();

	if (changed)
		baseTransform->CalculateMatrix();

	ImGui::Text("Frequency"); ImGui::SameLine();
	App->gui->DrawInputFloat("", "##frequency", &frequency, 0.1f, true);
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

void Particle::DisplayConfig()
{
	ImGui::Text("Particle");
	ImGui::NewLine();

	//Lifetime
	ImGui::Text("Lifetime"); ImGui::SameLine(75.0f);
	App->gui->DrawInputFloat("", "##lifetime", &lifeTime, 0.1f, true, &randLifeTime1, randomizeLifeTime); ImGui::SameLine();
	App->gui->DrawInputFloat("", "##randLifetime", &randLifeTime2, 0.1f, randomizeLifeTime);
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth()-70.0f);  ImGui::Checkbox("Random##lifetime", &randomizeLifeTime);
	ImGui::Separator();

	//Base transform
	///Billboard
	ImGui::Checkbox("Billboard", &baseTransform->billboard);
	///Rotation
	ImGui::Text("Rotation"); ImGui::SameLine(75.0f);
	App->gui->DrawInputFloat("", "##rotationZ", &baseTransform->angleZ, 1.0f, true, &baseTransform->randRotation1, baseTransform->randomizeRotation); ImGui::SameLine();
	App->gui->DrawInputFloat("", "##randRotationZ", &baseTransform->randRotation2, 1.0f, baseTransform->randomizeRotation);
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 70.0f);  ImGui::Checkbox("Random##rotation", &baseTransform->randomizeRotation);
	///Scale
	ImGui::Text("Scale"); ImGui::SameLine(75.0f);
	App->gui->DrawInputFloat("X", "##scaleX", &baseTransform->scale.x, 0.1f, true, &baseTransform->randScaleX1, baseTransform->randomizeScale); ImGui::SameLine();
	App->gui->DrawInputFloat("Y", "##scaleY", &baseTransform->scale.y, 0.1f, true, &baseTransform->randScaleY1, baseTransform->randomizeScale);
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 70.0f);  ImGui::Checkbox("Random##scale", &baseTransform->randomizeScale);
	ImGui::Text(""); ImGui::SameLine(75.0f);
	App->gui->DrawInputFloat("X", "##randScaleX", &baseTransform->randScaleX2, 0.1f, baseTransform->randomizeScale); ImGui::SameLine();
	App->gui->DrawInputFloat("Y", "##randScaleY", &baseTransform->randScaleY2, 0.1f, baseTransform->randomizeScale);
	ImGui::Separator();

	//Base movement
	ImGui::Text("Spped"); ImGui::SameLine(75.0f);
	App->gui->DrawInputFloat("", "##speed", &baseMovement->speed, 0.1f, true, &baseMovement->randSpeed1, baseMovement->randomizeSpeed); ImGui::SameLine();
	App->gui->DrawInputFloat("", "##randSpeed", &baseMovement->randSpeed2, 0.1f, baseMovement->randomizeSpeed);
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 70.0f);  ImGui::Checkbox("Random##speed", &baseMovement->randomizeSpeed);
	ImGui::Separator();

	//Base color
	ImGui::Text("Color"); ImGui::SameLine(75.0f);
	if (!baseColor->randomize)
		App->gui->DrawColorBox(baseColor->color);
	else
		App->gui->DrawGradientBox(baseColor->random);
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 70.0f);  ImGui::Checkbox("Random##color", &baseColor->randomize);
}

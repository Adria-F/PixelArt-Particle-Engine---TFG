#include "Application.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "Shader.h"
#include "ModuleCamera.h"

//Include all particle data nodes
#include "ParticleData.h"
#include "BaseTransform.h"
#include "BaseMovement.h"
#include "BaseColor.h"

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

	emitters.push_back(new ParticleEmitter());

	return true;
}

update_state ModuleParticles::Update(float dt)
{
	//TEMP: Debug
	ImGui::Begin("Particles");

	if (ImGui::Button("Play"))
		Play();
	if (ImGui::Button("Pause"))
		Pause();
	if (ImGui::Button("Stop"))
		Stop();

	ImGui::End();

	for (std::list<ParticleEmitter*>::iterator it_e = emitters.begin(); it_e != emitters.end(); ++it_e)
	{
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

// ---------------------- PARTICLE EMITTER --------------------------

ParticleEmitter::ParticleEmitter(Particle* templateParticle) :templateParticle(templateParticle)
{
	//TODO: Particle Emitter MUST receive a template particle
	if (templateParticle == nullptr)
	{
		this->templateParticle = new Particle();
	}
	lastEmit = frequency;
}

ParticleEmitter::~ParticleEmitter()
{
	RELEASE(templateParticle);

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

	//Delete all particles
	for (std::list<Particle*>::iterator it_p = particles.begin(); it_p != particles.end(); ++it_p)
	{
		RELEASE((*it_p));
	}
	particles.clear();
}

void ParticleEmitter::UpdateParticles(float dt)
{
	frequency = 0.25f;
	if (playing)
	{
		lastEmit += dt;

		if (lastEmit >= frequency)
		{
			lastEmit = 0.0f;
			Particle* part = new Particle(templateParticle);
			part->baseMovement->direction = randomDirectionInCone(3.0f, 5.0f);
			particles.push_back(part);
		}
	}

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
			it_p++;
	}
}

void ParticleEmitter::DrawParticles()
{
	for (std::list<Particle*>::iterator it_p = particles.begin(); it_p != particles.end(); ++it_p)
	{
		(*it_p)->Draw();
	}
}

vec ParticleEmitter::randomDirectionInCone(float radius, float height) const
{
	float angle = 2.0*PI * GET_RANDOM();
	float x = Cos(angle)*radius;
	float z = Sin(angle)*radius;

	vec point = vec(x, height, z);

	return point.Normalized();
}

// ----------------------------- PARTICLE ----------------------------------

Particle::Particle()
{
	for (int i = 0; i < MAX_PARTICLE_DATA; ++i)
	{
		data[i] = nullptr;
	}

	baseTransform = new BaseTransform(this);
	baseMovement = new BaseMovement(this);
	baseColor = new BaseColor(this);
}

Particle::Particle(Particle* templateParticle)
{
	for (int i = 0; i < MAX_PARTICLE_DATA; ++i)
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
	for (int i = 0; i < MAX_PARTICLE_DATA; ++i)
	{
		RELEASE(data[i]);
	}
}

void Particle::Update(float dt)
{
	timeAlife += dt;

	if (timeAlife >= lifeTime)
		toDestroy = true;

	for (int i = 0; i < MAX_PARTICLE_DATA; ++i)
	{
		if (data[i] != nullptr)
			data[i]->Execute(dt);
	}
}

void Particle::Draw()
{
	for (int i = 0; i < MAX_PARTICLE_DATA; ++i)
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

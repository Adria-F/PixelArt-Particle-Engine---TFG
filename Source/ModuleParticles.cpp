#include "Application.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "Shader.h"
#include "ModuleCamera.h"

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
	ImGui::Begin("Particles");

	if (ImGui::Button("Play"))
		Play();
	if (ImGui::Button("Pause"))
		Pause();
	if (ImGui::Button("Stop"))
		Stop();

	ImGui::End();

	for (int i = 0; i < emitters.size(); ++i)
	{
		emitters[i]->UpdateParticles(dt);
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
	for (int i = 0; i < emitters.size(); ++i)
	{
		emitters[i]->Play();
	}
}

void ModuleParticles::Pause()
{
	for (int i = 0; i < emitters.size(); ++i)
	{
		emitters[i]->Pause();
	}
}

void ModuleParticles::Stop()
{
	for (int i = 0; i < emitters.size(); ++i)
	{
		emitters[i]->Stop();
	}
}

void ModuleParticles::DrawParticles()
{
	for (int i = 0; i < emitters.size(); ++i)
	{
		emitters[i]->DrawParticles();
	}
}

// ---------------------- PARTICLE EMITTER --------------------------

ParticleEmitter::~ParticleEmitter()
{
	RELEASE(templateParticle);

	for (int i = 0; i < particles.size(); ++i)
	{
		RELEASE(particles[i]);
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
	for (int i = 0; i < particles.size(); ++i)
	{
		RELEASE(particles[i]);
	}
	particles.clear();
}

void ParticleEmitter::UpdateParticles(float dt)
{
	if (playing)
	{
		lastEmit += dt;

		if (lastEmit >= frequency)
		{
			lastEmit = 0.0f;
			particles.push_back(new Particle());
		}
	}

	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i]->LookCamera();
		if (playing)
			particles[i]->Update(dt);
	}
}

void ParticleEmitter::DrawParticles()
{
	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i]->Draw();
	}
}

// ----------------------------- PARTICLE ----------------------------------

void Particle::LookCamera()
{
	if (!billboard)
		return;

	vec cameraPos = App->camera->position;
	//caluclate direction to look
	vec dir = cameraPos - position;
	
	//caluclate the new rotation matrix
	float3x3 rotMat = float3x3::LookAt(vec(0.0f,0.0f,1.0f), dir.Normalized(), vec(0.0f,1.0f,0.0f), vec(0.0f, 1.0f, 0.0f));

	//set new rotation quaternion
	rotation = rotMat.ToQuat();
}

void Particle::Update(float dt)
{
	position.y += 2.0f*dt;
}

void Particle::Draw()
{
	//TODO: Access shader here?
	float4x4 matrix;
	matrix.Set(float4x4::FromTRS(position, rotation, scale));
	matrix.Transpose();
	App->render->defaultShader->sendMat4("model", (float*)matrix.v);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
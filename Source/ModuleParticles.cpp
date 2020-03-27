#include "Application.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "Shader.h"

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
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	rectangleVAO = App->render->generateVAO(sizeof(vertices), &vertices[0], sizeof(indices), &indices[0]);

	//As it is the only shape we are drawing, we can left it binded
	glBindVertexArray(rectangleVAO);

	emitters.push_back(new ParticleEmitter());

	return true;
}

update_state ModuleParticles::Update()
{
	for (int i = 0; i < emitters.size(); ++i)
	{
		emitters[i]->UpdateParticles();
	}

	return update_state::UPDATE_CONTINUE;
}

bool ModuleParticles::CleanUp()
{
	//TODO: Remove VAO

	return true;
}

void ModuleParticles::DrawParticles()
{
	for (int i = 0; i < emitters.size(); ++i)
	{
		emitters[i]->DrawParticles();
	}
}

ParticleEmitter::~ParticleEmitter()
{
	RELEASE(templateParticle);

	for (int i = 0; i < particles.size(); ++i)
	{
		RELEASE(particles[i]);
	}
	particles.clear();
}

void ParticleEmitter::UpdateParticles()
{
	frequency += 1.0f;
	if (particles.size() == 0 || frequency >= 60.0f)
	{
		frequency = 0.0f;
		particles.push_back(new Particle());
	}

	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i]->Update();
	}
}

void ParticleEmitter::DrawParticles()
{
	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i]->Draw();
	}
}

void Particle::Update()
{
	position.y += 0.05f;
}

void Particle::Draw()
{
	//TODO: Access shader here?
	static float4x4 matrix;
	matrix = { {1.0f,0.0f,0.0f,position.x},{0.0f,1.0f,0.0f,position.y},{0.0f,0.0f,1.0f,position.z},{0.0f,0.0f,0.0f,1.0f} };
	//matrix.Transpose();
	App->render->defaultShader->sendMat4("model", (float*)matrix.v);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

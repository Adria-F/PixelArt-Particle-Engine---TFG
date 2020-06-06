#include "Application.h"
#include "SpriteParticleNode.h"

#include "ModuleRender.h"
#include "Shader.h"
#include "ModuleTextures.h"

SpriteParticleNode::SpriteParticleNode(Particle* particle, const char* name, float2 position, float2 size) : EntityData(particle), CanvasNode(name, PARTICLE_SPRITE, position, size)
{
	GL_id = App->textures->UseWhiteTexture();
}

SpriteParticleNode::~SpriteParticleNode()
{
	App->textures->UnuseTexture(spriteUID);
}

void SpriteParticleNode::PrepareRender()
{
	BROFILER_CATEGORY("SpriteParticleNode", Profiler::Color::DarkKhaki);
	App->render->defaultShader->sendTexture("sprite", GL_id);
}

EntityData* SpriteParticleNode::Copy(Particle* particle) const
{
	SpriteParticleNode* ret = new SpriteParticleNode(particle, name.c_str(), position, size);

	ret->spriteUID = spriteUID;
	ret->GL_id = GL_id;

	App->textures->UseTexture(spriteUID); //New node will be using it

	return ret;
}

void SpriteParticleNode::DisplayConfig()
{
	if (ImGui::BeginCombo("Sprite", currentSprite.c_str()))
	{
		if (ImGui::Selectable("None"))
		{
			if (spriteUID != 0)
				App->textures->UnuseTexture(spriteUID);
			spriteUID = 0;
			GL_id = App->textures->UseWhiteTexture();
			currentSprite = "None";
		}
		for (std::map<uint, Texture*>::iterator it_t = App->textures->textures.begin(); it_t != App->textures->textures.end(); ++it_t)
		{
			if (ImGui::Selectable((*it_t).second->name.c_str()))
			{
				if (spriteUID != 0)
					App->textures->UnuseTexture(spriteUID);
				spriteUID = (*it_t).first;
				GL_id = App->textures->UseTexture(spriteUID);
				currentSprite = (*it_t).second->name;				
			}
		}

		ImGui::EndCombo();
	}

	float size = min(ImGui::GetWindowContentRegionWidth(), 256.0f);
	ImGui::Image((ImTextureID)GL_id, { size, size }, { 0,1 }, { 1,0 });
}

void SpriteParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addString("sprite", App->textures->GetTextureName(spriteUID).c_str());
}

void SpriteParticleNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	std::string spriteName = nodeDef->getString("sprite");

	spriteUID = App->textures->GetTextureUID(spriteName.c_str());
	GL_id = App->textures->UseTexture(spriteUID);
	currentSprite = spriteName;
}

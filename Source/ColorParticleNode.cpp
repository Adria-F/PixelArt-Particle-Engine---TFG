#include "Application.h"
#include "ColorParticleNode.h"

#include "ModuleParticles.h"
#include "BaseColorParticleNode.h"
#include "ModuleGUI.h"

ColorParticleNode::ColorParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_COLOR, position, size)
{
	fixColor = White;
}

void ColorParticleNode::Execute(float dt)
{
	if (overLifetime)
	{
		particle->baseColor->color = overLifetimeColor.GetColor(particle->GetLifePercent());
	}
	else
	{
		particle->baseColor->color = fixColor;
	}
}

EntityData* ColorParticleNode::Copy(Particle* particle) const
{
	ColorParticleNode* ret = new ColorParticleNode(particle, name.c_str(), position, size);

	ret->fixColor = fixColor;
	ret->overLifetimeColor = overLifetimeColor;

	ret->overLifetime = overLifetime;

	return ret;
}

void ColorParticleNode::DisplayConfig()
{
	if (ImGui::RadioButton("Fix", !overLifetime))
		overLifetime = false;
	ImGui::SameLine();
	if (ImGui::RadioButton("Over Lifetime", overLifetime))
		overLifetime = true;

	if (!overLifetime)
		App->gui->DrawColorBox(fixColor);
	else
		App->gui->DrawGradientBox(overLifetimeColor);
}

void ColorParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addVector4("color", fixColor.Get());

	//Save gradient
	JSON_Value* gradient = node->createValue();
	for (std::map<float, vec>::iterator it_c = overLifetimeColor.colorList.begin(); it_c != overLifetimeColor.colorList.end(); ++it_c)
	{
		JSON_Value* key = gradient->createValue();

		key->addUint("type", 0); //0 for color - 1 for alpha
		key->addFloat("percent", (*it_c).first);
		key->addVector3("color", (*it_c).second);

		gradient->addValue("", key);
	}
	for (std::map<float, float>::iterator it_a = overLifetimeColor.alphaList.begin(); it_a != overLifetimeColor.alphaList.end(); ++it_a)
	{
		JSON_Value* key = gradient->createValue();

		key->addUint("type", 1); //0 for color - 1 for alpha
		key->addFloat("percent", (*it_a).first);
		key->addFloat("alpha", (*it_a).second);

		gradient->addValue("", key);
	}
	node->addValue("gradient", gradient);

	node->addBool("overLifetime", overLifetime);
}

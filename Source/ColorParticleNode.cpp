#include "Application.h"
#include "ColorParticleNode.h"

#include "ModuleParticles.h"
#include "BaseColorParticleNode.h"
#include "ModuleGUI.h"

ColorParticleNode::ColorParticleNode(Particle* particle, const char* name, float2 position, float2 size): EntityData(particle), CanvasNode(name, PARTICLE_COLOR, position, size)
{
	update = true;
	fixColor = White;
}

void ColorParticleNode::Init()
{
	particle->baseColor->color = fixColor;
}

void ColorParticleNode::Execute(float dt)
{
	BROFILER_CATEGORY("ColorParticleNode", Profiler::Color::LightGray);

	particle->baseColor->color = overLifetimeColor.GetColor(particle->GetLifePercent());
}

EntityData* ColorParticleNode::Copy(Particle* particle) const
{
	ColorParticleNode* ret = new ColorParticleNode(particle, name.c_str(), position, size);

	ret->fixColor = fixColor;
	ret->overLifetimeColor = overLifetimeColor;

	ret->update = update;

	return ret;
}

void ColorParticleNode::DisplayConfig()
{
	if (ImGui::RadioButton("Fix", !update))
		update = false;
	ImGui::SameLine();
	if (ImGui::RadioButton("Over Lifetime", update))
		update = true;

	if (!update)
		App->gui->DrawColorBox(fixColor);
	else
		App->gui->DrawGradientBox(overLifetimeColor);
}

void ColorParticleNode::SaveExtraInfo(JSON_Value* node)
{
	node->addVector4("color", fixColor.Get());

	//Save gradient
	JSON_Value* gradient = node->createValue();
	gradient->convertToArray();
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

	node->addBool("overLifetime", update);
}

void ColorParticleNode::LoadExtraInfo(JSON_Value* nodeDef)
{
	float4 color = nodeDef->getVector4("color");
	fixColor.Set(color.x, color.y, color.z, color.w);

	overLifetimeColor.RemoveAllKeys(); //First remove the default keys
	JSON_Value* gradient = nodeDef->getValue("gradient");
	for (int i = 0; i < gradient->getRapidJSONValue()->Size(); ++i)
	{
		JSON_Value* key = gradient->getValueFromArray(i);
		uint type = key->getUint("type");
		float percent = key->getFloat("percent");

		if (type == 0) //Color
		{
			vec color = key->getVector3("color");
			overLifetimeColor.SetColor(color, percent);
		}
		else if (type == 1) //Alpha
		{
			float alpha = key->getFloat("alpha");
			overLifetimeColor.SetAlpha(alpha, percent);
		}
	}

	update = nodeDef->getBool("overLifetime");
}

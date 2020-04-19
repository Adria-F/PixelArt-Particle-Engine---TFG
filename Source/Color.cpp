#include "Color.h"
#include "Globals.h"

Color Red = Color(1.0f, 0.0f, 0.0f);
Color Green = Color(0.0f, 1.0f, 0.0f);
Color Blue = Color(0.0f, 0.0f, 1.0f);
Color Black = Color(0.0f, 0.0f, 0.0f);
Color White = Color(1.0f, 1.0f, 1.0f);
Color lightGray = Color(0.5f, 0.5f, 0.5f);
Color Gray = Color(0.3f, 0.3f, 0.3f);
Color Yellow = Color(1.0f, 0.9f, 0.0f);
Color Orange = Color(1.0f, 0.5f, 0.0f);

Gradient::Gradient()
{
	colorList.insert(std::pair<float, vec>(0.0f, White.rgb));
	colorList.insert(std::pair<float, vec>(1.0f, Black.rgb));

	alphaList.insert(std::pair<float, float>(0.0f, 1.0f));
	alphaList.insert(std::pair<float, float>(1.0f, 1.0f));
}

void Gradient::SetColor(vec color, float percent)
{
	if (percent >= 0.0f && percent <= 1.0f)
		colorList[percent] = color;
}

void Gradient::SetAlpha(float alpha, float percent)
{
	if (percent >= 0.0f && percent <= 1.0f)
		alphaList[percent] = alpha;
}

Color Gradient::GetColor(float percent)
{
	Color ret;

	std::map<float, vec>::iterator prevColor = colorList.begin();
	std::map<float, vec>::iterator nextColor;
	for (nextColor = colorList.begin(); nextColor != colorList.end(); ++nextColor)
	{
		if ((*nextColor).first < percent)
		{
			prevColor = nextColor;
		}
		else if ((*nextColor).first > percent)
		{
			break;
		}
		else
		{
			prevColor = nextColor;
			break;
		}
	}

	std::map<float, float>::iterator prevAlpha = alphaList.begin();
	std::map<float, float>::iterator nextAlpha;
	for (nextAlpha = alphaList.begin(); nextAlpha != alphaList.end(); ++nextAlpha)
	{
		if ((*nextAlpha).first < percent)
		{
			prevAlpha = nextAlpha;
		}
		else if ((*nextAlpha).first > percent)
		{
			break;
		}
		else
		{
			prevAlpha = nextAlpha;
			break;
		}
	}

	float colorDelta = 0.0f;
	if ((*prevColor).first != (*nextColor).first)
		colorDelta = (1.0f/((*nextColor).first - (*prevColor).first))*(percent- (*prevColor).first);
	
	float alphaDelta = 0.0f;
	if ((*prevAlpha).first != (*nextAlpha).first)
		alphaDelta = (1.0f / ((*nextAlpha).first - (*prevAlpha).first))*(percent - (*prevAlpha).first);

	if (nextColor == colorList.end())
		ret.rgb = (*prevColor).second;
	else
		ret.rgb = Lerp((*prevColor).second, (*nextColor).second, colorDelta);
	if (nextAlpha == alphaList.end())
		ret.a = (*prevAlpha).second;
	else
		ret.a = Lerp((*prevAlpha).second, (*nextAlpha).second, alphaDelta);

	return ret;
}

bool Gradient::hasColorKey(float percent) const
{
	return colorList.find(percent) != colorList.end();
}

bool Gradient::hasAlphaKey(float percent) const
{
	return alphaList.find(percent) != alphaList.end();
}

void Gradient::MoveColorKey(float originalPercent, float newPercent)
{
	if (newPercent >= 0.0f && newPercent <= 1.0f && colorList.find(originalPercent) != colorList.end())
	{
		vec color = colorList.at(originalPercent);
		colorList.erase(originalPercent);
		colorList.insert(std::pair<float, vec>(newPercent, color));
	}
}

void Gradient::MoveAlphaKey(float originalPercent, float newPercent)
{
	if (newPercent >= 0.0f && newPercent <= 1.0f && alphaList.find(originalPercent) != alphaList.end())
	{
		float alpha = alphaList.at(originalPercent);
		alphaList.erase(originalPercent);
		alphaList.insert(std::pair<float, float>(newPercent, alpha));
	}
}

void Gradient::RemoveColorKey(float percent)
{
	colorList.erase(percent);
}

void Gradient::RemoveAlphaKey(float percent)
{
	alphaList.erase(percent);
}

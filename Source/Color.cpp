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

Gradient::Gradient(vec baseColor)
{
	colorList.insert(std::pair<float, vec>(0.0f, baseColor));
	colorList.insert(std::pair<float, vec>(1.0f, baseColor));

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

	std::map<float, vec>::iterator prevColor;
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

	std::map<float, float>::iterator prevAlpha;
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
		colorDelta = percent / ((*prevColor).first + (*nextColor).first);
	
	float alphaDelta = 0.0f;
	if ((*prevAlpha).first != (*nextAlpha).first)
		alphaDelta = percent / ((*prevAlpha).first + (*nextAlpha).first);

	ret.rgb = Lerp((*prevColor).second, (*nextColor).second, colorDelta);
	ret.a = Lerp((*prevAlpha).second, (*nextAlpha).second, alphaDelta);

	return ret;
}
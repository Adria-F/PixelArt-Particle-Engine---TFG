#pragma once

#include "Globals.h"
#include <map>

struct Color
{
	vec rgb;
	float a;
	
	Color() : rgb({ 0.0f,0.0f,0.0f }), a(0.0f)
	{}

	Color(float r, float g, float b, float a = 1.0f) : rgb({ r,g,b }), a(a)
	{}

	void Set(float r, float g, float b, float a = 1.0f)
	{
		this->rgb = vec(r, g, b);
		this->a = a;
	}

	float4 Get() const
	{

		return float4({ rgb, a });
	}

	float* operator & ()
	{
		return (float*)this;
	}
};

extern Color Red;
extern Color Green;
extern Color Blue;
extern Color Black;
extern Color White;
extern Color Gray;
extern Color lightGray;
extern Color Yellow;
extern Color Orange;

struct Gradient
{
	Gradient() : Gradient(White.rgb) {}
	Gradient(vec baseColor);

	void SetColor(vec color, float percent);
	void SetAlpha(float alpha, float percent);
	Color GetColor(float percent);

	bool hasKey(float percent) const;
	void MoveColorKey(float originalPercent, float newPercent);

	std::map<float, vec> colorList;
	std::map<float, float> alphaList;
};
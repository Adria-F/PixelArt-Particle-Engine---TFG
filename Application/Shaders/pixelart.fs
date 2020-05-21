#version 330 core
layout(location = 1) out vec4 outPixelized;

uniform sampler2D scene;
uniform vec2 viewportSize;
uniform unsigned int pixelSize;

in vec2 texCoords;

void main()
{
	vec2 tile = floor(viewportSize*texCoords/pixelSize);   
	vec2 uv = tile*pixelSize/viewportSize;
	outPixelized = vec4(texture(scene, uv).xyz,1.0);
}
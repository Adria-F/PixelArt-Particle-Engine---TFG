#version 330 core
layout(location = 0) out vec4 color;

uniform vec4 objectColor;
uniform sampler2D sprite;

in vec2 texCoords;

void main()
{
    color = texture(sprite, texCoords) * objectColor;
}
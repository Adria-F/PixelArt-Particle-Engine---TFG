#version 330 core
layout(location = 0) out vec4 outPixelized;

uniform sampler2D scene;
uniform vec2 viewportSize;
uniform unsigned int pixelSize;

in vec2 texCoords;

void main()
{
	vec2 tile = floor(viewportSize*texCoords/pixelSize);   

	vec2 topLeft = tile*pixelSize;

	vec4 color = vec4(1.0);
	for (float i = topLeft.x; i < topLeft.x+pixelSize; i++)
	{
		if (i >= viewportSize.x)
		{
			break;
		}

		for (float j = topLeft.y; j < topLeft.y+pixelSize; j++)
		{
			if (j >= viewportSize.y)
			{
				break;
			}

			color += texture(scene, (vec2(i,j)/viewportSize));
		}
	}

	outPixelized = color/(pixelSize*pixelSize);
}
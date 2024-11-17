#version 330 core
out vec4 finalColor;

in vec2 texCoord;

uniform sampler2D color;
uniform sampler2D depth;

const vec3 skyColor = vec3(0.5, 0.5, 1.0);

void main()
{
	finalColor = vec4(skyColor, 1.0);
}
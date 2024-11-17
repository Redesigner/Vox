#version 330 core
out vec4 finalColor;

in vec2 texCoord;
in vec4 vertexWS;

uniform sampler2D color;

const vec3 skyColor = vec3(0.4, 0.4, 1.0);
const vec3 groundColor = vec3(0.3, 0.3, 0.3);
void main()
{
	vec4 normal = normalize(vertexWS);
	float alpha = pow(normal.y + 0.2, 0.5);
	finalColor = vec4(mix(groundColor, skyColor, clamp(alpha, 0.0, 1.0)), 1.0);
	// finalColor = vec4(normal.xyz, 1.0);
}
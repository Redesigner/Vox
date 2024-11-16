#version 330 core
out vec4 finalColor;

in vec2 texCoord;

uniform sampler2D color;
uniform sampler2D depth;

const vec3 skyColor = vec3(0.5, 0.5, 1.0);

void main()
{
	// float depth = texture(depth, texCoord).r;
	float depth = gl_FragDepth;
	if (depth >= 1.0)
	{
		finalColor = vec4(skyColor, 1.0);
	}
	else
	{
		vec4(texture(color, texCoord).rgb, 1.0);
	}

	finalColor = vec4(depth, 0, 0, 1.0);
}
#version 330 core
out vec4 finalColor;

in vec2 texCoord;
in vec4 vertexWS;

uniform sampler2D color;

const vec3 groundColor = vec3(0.3, 0.3, 0.3);
const vec3 sunPos = normalize(vec3(1.0, 0.6, 0.0));

const float Br = 0.0005;
const float Bm = 0.0003;
const float g =  0.9200;
const vec3 nitrogen = vec3(0.650, 0.570, 0.475);
const vec3 Kr = Br / pow(nitrogen, vec3(4.0));
const vec3 Km = Bm / pow(nitrogen, vec3(0.84));

void main()
{
	vec4 pos = normalize(vertexWS);
	pos.y += 0.05;
	pos = normalize(pos);
	float mu = dot(pos.xyz, normalize(sunPos));
	float rayleigh = (3.0 / (8.0 * 3.14)) * (1.0 + mu * mu);
	vec3 mie = (Kr + Km * (1.0 - g * g) / (2.0 + g * g) / pow(1.0 + g * g - 2.0 * g * mu, 1.5)) / (Br + Bm);
    vec3 extinction = exp(-exp(-((pos.y + sunPos.y * 4.0) * (exp(-pos.y * 16.0) + 0.1) / 80.0) / Br) * (exp(-pos.y * 16.0) + 0.1) * Kr / Br) * exp(-pos.y * exp(-pos.y * 8.0 ) * 4.0) * exp(-pos.y * 2.0) * 4.0;
	finalColor = vec4( pos.y > 0.0 ? (rayleigh * mie * extinction) * 0.1 : groundColor, 1.0);
}
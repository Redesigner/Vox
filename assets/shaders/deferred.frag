#version 330 core
out vec4 finalColor;
out float depth;
in vec2 texCoord;
in vec2 texCoord2;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallicRoughness;
uniform sampler2D gDepth;

struct Light {
    int enabled;
    int type; // Unused in this demo.
    vec3 position;
    vec3 target; // Unused in this demo.
    vec4 color;
    float strength;
};

const int NR_LIGHTS = 4;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPosition;

const float QUADRATIC = 0.032;
const float LINEAR = 0.09;
const float PI = 3.14159265;

float normalDistribution(vec3 normal, vec3 halfway, float roughness) 
{
    float roughnessSquared = roughness * roughness;
    float nH = dot(normal, halfway);
    float denominator = (nH * nH * (roughnessSquared - 1) ) + 1;
    return (roughnessSquared) / (PI * denominator * denominator);
}

float geometric(vec3 normal, vec3 viewDirection, float roughness)
{
    float normalDot = dot(normal, viewDirection);
    float k = ((roughness + 1) * (roughness + 1)) / 8.0;
    return normalDot / ((normalDot * (1.0 - k)) + k);
}

vec3 fresnel(vec3 normal, vec3 lightDirecion, vec3 color)
{
    float normalDot = dot(normal, lightDirecion);
    return color + (1.0 - color) * pow(1.0 - normalDot, 5);
}

vec3 cookTorrance(vec3 normal, vec3 halfwayDirection, vec3 lightDirection, vec3 viewDirection, float roughness, vec3 color)
{
    return 
    (normalDistribution(normal, halfwayDirection, roughness)
    * geometric(normal, viewDirection, roughness)
    * fresnel(normal, lightDirection, color))
    / (4.0 * dot(normal, lightDirection) * dot(normal, viewDirection));
}

void main() {
    vec3 fragPosition = texture(gPosition, texCoord).rgb;
    vec3 normal = texture(gNormal, texCoord).rgb;
    vec3 albedo = texture(gAlbedo, texCoord).rgb;
    float roughness = texture(gMetallicRoughness, texCoord).g;
    float metallic = texture(gMetallicRoughness, texCoord).r;

    vec3 ambient = albedo * vec3(0.1f);
    vec3 viewDirection = normalize(viewPosition - fragPosition);

    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        if(lights[i].enabled == 0) continue;
        vec3 lightDirection;
        switch(lights[i].type)
        {
            case 0:
                lightDirection = lights[i].position - fragPosition;
            break;
            case 1:
                lightDirection = normalize(lights[i].position);
            break;
        }
        vec3 diffuse = max(dot(normal, lightDirection), 0.0) * albedo * lights[i].color.xyz;

        vec3 halfwayDirection = normalize(lightDirection + viewDirection);
        vec3 lightContribution = cookTorrance(normal, halfwayDirection, lightDirection, viewDirection, roughness, lights[i].color.rgb);

        // Attenuation
        float distance = length(lights[i].position - fragPosition);
        float attenuation = 1.0 / (1.0 + LINEAR * distance + QUADRATIC * distance * distance);
        lightContribution *= attenuation;
        ambient += lightContribution;
    }

    finalColor = vec4(ambient, 1.0);
    // finalColor = vec4(lights[0].position, 1.0);
    // finalColor = vec4(matSpecular, 0.0, 0.0, 1.0);
    depth = texture(gDepth, texCoord).r;
}
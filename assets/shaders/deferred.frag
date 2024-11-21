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

uniform vec3 viewPosition;

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

const float QUADRATIC = 0.032;
const float LINEAR = 0.09;
const float PI = 3.14159265;

float extinction(vec3 normalA, vec3 normalB)
{
    return ceil(max(dot(normalA, normalB), 0.0));
}

float normalDistribution(vec3 normal, vec3 halfway, float alpha) 
{
    float alphaSquared = alpha * alpha;
    float nH = dot(normal, halfway);
    float denominator = (nH * nH * (alphaSquared - 1) ) + 1;
    return (alphaSquared) / (PI * denominator * denominator);
}

float geometric(vec3 normal, vec3 viewDirection, float alpha)
{
    float normalDot = dot(normal, viewDirection);
    float k = ((alpha + 1) * (alpha + 1)) / 8.0;
    return normalDot / ((normalDot * (1.0 - k)) + k);
}

float fresnel(vec3 viewDirection, vec3 halfway)
{
    const float f0 = 0.7;
    float vh = dot(viewDirection, halfway);
    return f0 + (1.0 - f0) * pow(2, (-5.55473 * vh - 6.98316) * vh);
}

float cookTorrance(vec3 normal, vec3 lightDirection, vec3 viewDirection, float roughness)
{
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    float alpha = roughness * roughness;
    float D = normalDistribution(normal, halfwayDirection, alpha);
    float G = geometric(normal, viewDirection, alpha);
    float F = fresnel(viewDirection, halfwayDirection);
    // return (D * G * F) / (4.0 * dot(normal, lightDirection) * dot(normal, viewDirection));
    // return vec3(D, G, F);
    return D * G * F;
}

void main() {
    vec3 fragPosition = texture(gPosition, texCoord).rgb;
    vec3 normal = texture(gNormal, texCoord).rgb;
    vec3 albedo = texture(gAlbedo, texCoord).rgb;
    // float roughness = texture(gMetallicRoughness, texCoord).g;
    float roughness = 0.1;
    float metallic = texture(gMetallicRoughness, texCoord).r;

    vec3 ambient = albedo * vec3(0.1f);
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    // vec3 viewDirection = normalize(viewPosition);


    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        if(lights[i].enabled == 0) continue;
        vec3 lightDirection;

        float attenuation = 1.0;

        switch(lights[i].type)
        {
            // Point light
            case 0:
                lightDirection = normalize(lights[i].position - fragPosition);
                float distance = length(lights[i].position - fragPosition);
                attenuation = 1.0 / (1.0 + LINEAR * distance + QUADRATIC * distance * distance);
            break;

            // Directional light
            case 1:
                lightDirection = normalize(lights[i].position);
            break;
        }
        vec3 diffuse = max(dot(normal, lightDirection), 0.0) * albedo * lights[i].color.xyz;

        vec3 specular = cookTorrance(normal, lightDirection, viewDirection, roughness) * lights[i].color.rgb;
        diffuse *= attenuation;
        specular *= attenuation;
        ambient += diffuse + specular;
    }

    finalColor = vec4(ambient, 1.0);
    depth = texture(gDepth, texCoord).r;
}
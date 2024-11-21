#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexCoord;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in int vertexTextureId;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out int fragTextureId;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main()
{
    vec4 worldPos = matModel * vec4(vertexPosition, 1.0);
    fragPosition = worldPos.xyz; 
    fragTexCoord = vertexTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(matModel)));
    fragNormal = normalMatrix * vertexNormal;
    fragTextureId = vertexTextureId;

    gl_Position = matProjection * matView * worldPos;
}
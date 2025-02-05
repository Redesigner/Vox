#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in ivec4 vertexJoints;
layout (location = 4) in vec4 vertexWeights;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

uniform data
{
    mat4 transform[64];
} joints;

void main()
{
    mat4 matBonesCombined = 
        joints.transform[vertexJoints.x] * vertexWeights.x +
        joints.transform[vertexJoints.y] * vertexWeights.y +
        joints.transform[vertexJoints.z] * vertexWeights.z +
        joints.transform[vertexJoints.w] * vertexWeights.w;

    mat4 matSkin = matModel * matBonesCombined;
    vec4 worldPos = matSkin * vec4(vertexPosition, 1.0);
    fragPosition = worldPos.xyz; 
    fragTexCoord = vertexTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(matSkin)));
    fragNormal = normalMatrix * vertexNormal;

    gl_Position = matProjection * matView * worldPos;
}
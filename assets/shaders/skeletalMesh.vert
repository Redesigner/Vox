#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in vec4 vertexJoints;
layout (location = 4) in vec4 vertexWeights;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

uniform mat4[64] matJoints;

void main()
{
    mat4 matBonesCombined = 
        matJoints[int(vertexJoints.x)] * vertexWeights.x +
        matJoints[int(vertexJoints.y)] * vertexWeights.y +
        matJoints[int(vertexJoints.z)] * vertexWeights.z +
        matJoints[int(vertexJoints.w)] * vertexWeights.w;

    mat4 matSkin = matModel * matBonesCombined;
    vec4 worldPos = matSkin * vec4(vertexPosition, 1.0);
    fragPosition = worldPos.xyz; 
    fragTexCoord = vertexTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(matSkin)));
    fragNormal = normalMatrix * vertexNormal;

    gl_Position = matProjection * matView * worldPos;
}
#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;
layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

out vec4 vCol;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 vColor;

uniform mat4 finalBonesMatrices[MAX_BONES];
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 color;
uniform vec2 toffset;

void main()
{
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal = vec3(0.0);
    float totalWeight = 0.0;

    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (boneIDs[i] == -1)
            continue;
        if (boneIDs[i] >= MAX_BONES)
            continue;

        vec4 localPosition = finalBonesMatrices[boneIDs[i]] * vec4(pos, 1.0);
        totalPosition += localPosition * weights[i];
        totalNormal += mat3(finalBonesMatrices[boneIDs[i]]) * norm * weights[i];
        totalWeight += weights[i];
    }

    if (totalWeight <= 0.0)
    {
        totalPosition = vec4(pos, 1.0);
        totalNormal = norm;
    }

    gl_Position = projection * view * model * totalPosition;
    vCol = vec4(0.0, 1.0, 0.0, 1.0);
    vColor = vec4(color, 1.0);
    TexCoord = tex + toffset;
    Normal = mat3(transpose(inverse(model))) * totalNormal;
    FragPos = (model * totalPosition).xyz;
}

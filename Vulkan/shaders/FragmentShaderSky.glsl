#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_UVS 2
#define BINDING_TEX 4

//input
layout(location = BINDING_UVS) in vec3 texCoord;

layout(location = 0) out vec4 outColor;

layout(set=0, binding = BINDING_TEX) uniform samplerCube texSampler;


void main()
{
    outColor = texture(texSampler, texCoord);
}
#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_POS 0
#define BINDING_NOR 1
#define BINDING_UVS 2
#define BINDING_UBO 3
#define BINDING_TEX 4

layout(set=1, binding = BINDING_POS) buffer pos 
{
	vec4 position_in[];
};

layout(set=1, binding = BINDING_NOR) buffer nor 
{
	vec4 normals_in[];
};

layout(set=1, binding = BINDING_UVS) buffer inTexCoord { vec2 uv_in[]; };
layout(location = BINDING_UVS) out vec2 fragTexCoord;

layout(set=1, binding = BINDING_UBO) uniform UniformBufferObject
{
	mat4 model;
} ubo;

layout(set=0, binding=0) uniform UniformBufferObject2
{
	mat4 view;
	mat4 proj;
} ubo2;

void main() {
	gl_Position = ubo2.proj * ubo2.view * ubo.model * position_in[gl_VertexIndex];
    fragTexCoord = uv_in[gl_VertexIndex];
	vec4 jaFim = normals_in[gl_VertexIndex];
	float f = jaFim.x + 8;
	jaFim.y = f;
}

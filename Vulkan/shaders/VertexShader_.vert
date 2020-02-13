#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_POS 0
#define BINDING_NOR 1
#define BINDING_UVS 2
#define BINDING_UBO 3
#define BINDING_TEX 4
#define BINDING_UCL 0

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

layout(set=1, binding = BINDING_UBO) uniform UniformObject
{
	mat4 model;
} uo;

layout(set=0, binding = BINDING_UCL) uniform UniformCameraLight
{
	mat4 view;
	mat4 proj;
	vec4 dir;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 camPos;
} ucl;

out vec3 normal;

void main() {
	gl_Position = ucl.proj * ucl.view * uo.model * position_in[gl_VertexIndex];
    fragTexCoord = uv_in[gl_VertexIndex];
	normal = normals_in[gl_VertexIndex].xyz;
}

#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_POS 0
#define BINDING_NOR 1
#define BINDING_UVS 2
#define BINDING_UBO 3
#define BINDING_TEX 4
#define BINDING_UCL 0

layout(set=0, binding = BINDING_POS) buffer pos 
{
	vec4 position_in[];
};

layout(set=0, binding = BINDING_NOR) buffer nor 
{
	vec4 normals_in[];
};

layout(set=0, binding = BINDING_UVS) buffer inTexCoord { vec2 uv_in[]; };
layout(location = BINDING_UVS) out vec3 texCoord;

layout(set=0, binding = BINDING_UBO) uniform UniformObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
	vec4 camPos;
} uo;

void main()
{
	gl_Position = uo.proj * uo.view * position_in[gl_VertexIndex];
	gl_Position = gl_Position.xyww;
    texCoord = position_in[gl_VertexIndex].xyz;
}

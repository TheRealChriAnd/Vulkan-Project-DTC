#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_POS 0
#define BINDING_COLOR 1
#define BINDING_UV 2
#define BINDING_UNI 3
#define BINDING_TEXTURE 4

layout(set=1, binding=0) buffer pos 
{
	vec4 position_in[];
};

layout(set=1,binding=1) buffer inTexCoord { vec2 uv_in[]; };
layout(location=1) out vec2 fragTexCoord;

layout(set=1,binding=2) uniform UniformBufferObject
{
	mat4 model;
} ubo;

layout(set=0,binding=0) uniform UniformBufferObject2
{
	mat4 view;
	mat4 proj;
} ubo2;

void main() {
	gl_Position = ubo2.proj * ubo2.view * ubo.model * position_in[gl_VertexIndex];
    fragTexCoord = uv_in[gl_VertexIndex];
}

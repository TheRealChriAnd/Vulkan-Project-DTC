#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_POS 0
#define BINDING_COLOR 1
#define BINDING_UV 2
#define BINDING_UNI 3
#define BINDING_TEXTURE 4

// buffer inputs
layout(binding=BINDING_COLOR) buffer inColor { vec3 color_in[]; };
layout(location=BINDING_COLOR) out vec3 fragColor;

layout(binding=BINDING_UV) buffer inTexCoord { vec2 uv_in[]; };
layout(location=BINDING_UV) out vec2 fragTexCoord;

layout(binding=BINDING_POS) buffer pos { vec3 position_in[]; };

layout(binding=BINDING_UNI) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(position_in[gl_VertexIndex], 1.0);
    fragColor = color_in[gl_VertexIndex];
    fragTexCoord = uv_in[gl_VertexIndex];
}

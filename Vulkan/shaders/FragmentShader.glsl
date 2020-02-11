#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_COLOR 1
#define BINDING_UV 2
#define BINDING_TEXTURE 4

layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set=1,binding = 3) uniform sampler2D texSampler;

void main() {
    outColor = texture(texSampler, fragTexCoord);
}
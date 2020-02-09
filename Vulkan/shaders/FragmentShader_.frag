#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_COLOR 1
#define BINDING_UV 2
#define BINDING_TEXTURE 4
//input
layout(location = BINDING_COLOR) in vec4 fragColor;
layout(location = BINDING_UV) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = BINDING_TEXTURE) uniform sampler2D texSampler;

void main() {
    outColor = texture(texSampler, fragTexCoord) * fragColor;
}
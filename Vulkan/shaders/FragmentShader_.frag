#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_UVS 2
#define BINDING_TEX 4

layout(location = BINDING_UVS) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set=1, binding = BINDING_TEX) uniform sampler2D texSampler;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void main() {
    outColor = texture(texSampler, fragTexCoord) * fragColor;
}
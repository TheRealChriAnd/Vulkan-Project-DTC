#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_COLOR 1
#define BINDING_UVS 2
#define BINDING_TEX 4
#define BINDING_LIGHT 6

struct Light {
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

//input
layout(location = BINDING_COLOR) in vec4 fragColor;
layout(location = BINDING_UVS) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set=1, binding = BINDING_TEX) uniform sampler2D texSampler;

layout(binding=BINDING_LIGHT) uniform UBO
{
    Light light;
} ubo;

void main() {

    //ambient
    vec3 ambient = ubo.light.ambient * texture(||material.diffuse, fragTexCoord).rgb;

    //diffuse
    vec3 norm = normalize(||normal);
    vec3 lightDir = normalize(-ubo.light.direction);
    flaot diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = ubo.light.diffuse * diff * texture(||material.diffuse, fragTexCoord).rgb
    
    //specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), ||material.shininess);
    vec3 specular = light.specular * spec * texture(||material.specular, fragTexCoord).rgb;

    vec3 result = ambient + diffuse + specular;
    ourColor = vec4(result, 1.0);

   //outColor = texture(texSampler, fragTexCoord) * fragColor;
}
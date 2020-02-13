#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_NOR 1
#define BINDING_UVS 2
#define BINDING_TEX 4
#define BINDING_UCL 0

//input
layout(location = BINDING_UVS) in vec2 fragTexCoord;

layout(location = BINDING_NOR) in vec3 normal;
layout(location = 0) out vec4 outColor;

layout(set=1, binding = BINDING_TEX) uniform sampler2D texSampler;

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

void main()
 {

    //ambient
    //vec3 ambient = ucl.ambient * texture(vec3(1.0, 0.5, 0.31), fragTexCoord).rgb;
    vec3 ambient = ucl.ambient.xyz * vec3(1.0, 1.0, 1.0);
    
    //diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-ucl.dir.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = ucl.diffuse.xyz * diff * texture(vec3(1.0, 0.5, 0.31), fragTexCoord).rgb // material diffuse
    vec3 diffuse = ucl.ambient.xyz * (diff * vec3(1.0, 1.0, 1.0));
    
    //specular
    vec3 viewDir = normalize(ucl.camPos.xyz - gl_FragCoord.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    ////vec3 specular = ucl.specular.xyz * spec * texture(vec3(0.5, 0.5, 0.5), fragTexCoord).rgb;
    vec3 specular = ucl.ambient.xyz * (spec * vec3(0.5, 0.5, 0.5));

    vec3 result = ambient + diffuse + specular;
    outColor = texture(texSampler, fragTexCoord) * vec4(result, 1.0);
    //outColor = texture(texSampler, fragTexCoord) * vec4(diffuse, 1.0); // * fragColor;
}
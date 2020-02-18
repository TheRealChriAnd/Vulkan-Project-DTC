#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BINDING_NOR 1
#define BINDING_UVS 2
#define BINDING_TEX 4
#define BINDING_UCL 0
#define NR_POINT_LIIGHTS 5

struct PLight {
    vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 CQL;
};

//input
layout(location = BINDING_UVS) in vec2 fragTexCoord;

layout(location = BINDING_NOR) in vec3 normal;
layout(location = 0) out vec4 outColor;

layout(location = 10) in vec3 worldPos;


layout(set=1, binding = BINDING_TEX) uniform sampler2D texSampler;

layout(set=0, binding = BINDING_UCL) uniform UniformCameraLight
{
	mat4 view;
	mat4 proj;
    PLight lights[NR_POINT_LIIGHTS];
	vec4 camPos;
} ucl;


vec3 CalcPointLight(PLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.5);
    // attenuation
    float distance    = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.CQL.x + light.CQL.z * distance + 
    		    light.CQL.y * (distance * distance));         
    // combine results
    vec3 ambient  = light.ambient.xyz * vec3(1.0, 1.0, 1.0);
    vec3 diffuse  = light.diffuse.xyz * (diff * vec3(1.0, 1.0, 1.0));
    vec3 specular = light.specular.xyz * (spec * vec3(0.5, 0.5, 0.5));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

void main()
 {

    //ambient
    //vec3 ambient = ucl.ambient * texture(vec3(1.0, 0.5, 0.31), fragTexCoord).rgb;
    //vec3 ambient = ucl.ambient.xyz * vec3(1.0, 1.0, 1.0);
    
    //diffuse
    vec3 norm = normalize(normal);
    //vec3 lightDir = normalize(-ucl.dir.xyz);
    //float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = ucl.diffuse.xyz * diff * texture(vec3(1.0, 0.5, 0.31), fragTexCoord).rgb // material diffuse
    //vec3 diffuse = ucl.ambient.xyz * (diff * vec3(1.0, 1.0, 1.0));
    
    //specular
    vec3 viewDir = normalize(ucl.camPos.xyz - worldPos);
    //vec3 reflectDir = reflect(-lightDir, norm);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    ////vec3 specular = ucl.specular.xyz * spec * texture(vec3(0.5, 0.5, 0.5), fragTexCoord).rgb;
    //vec3 specular = ucl.ambient.xyz * (spec * vec3(0.5, 0.5, 0.5));
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < NR_POINT_LIIGHTS; i++)
    {
        result += CalcPointLight(ucl.lights[i], norm, worldPos, viewDir);
    }
    outColor = texture(texSampler, fragTexCoord) * vec4(result, 1.0);
    //outColor = texture(texSampler, fragTexCoord) * vec4(diffuse, 1.0); // * fragColor;
}
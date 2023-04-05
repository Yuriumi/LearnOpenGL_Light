#version 330 core
struct Material{
	// vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material material;

struct Light{
	vec3 position;
	vec3 direction;

	vec3 flashPosition;
	vec3 flashDirection;

	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform Light light;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPosition;

void main()
{
	vec3 norm = normalize(Normal);
	// vec3 lightDir = normalize(-light.direction);
	vec3 lightDir = normalize(light.flashPosition - FragPos);
	vec3 viewDir = normalize(viewPosition - FragPos);

	// Ambient Light 环境光颜色在几乎所有情况下都等于漫反射颜色
	 vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));

	// Diffuse Light
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = (diff * vec3(texture(material.diffuse,TexCoords))) * light.diffuse;

	// Specular Light
	float specularStrength = 0.5f;
	vec3 reflectDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular = (spec * vec3(texture(material.specular,TexCoords))) * light.specular;

	// Spot light attribute
	float theta = dot(lightDir, normalize(-light.flashDirection));
	float epsilon = (light.cutOff - light.outerCutOff);
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f,1.0f);
	diffuse *= intensity;
	specular *= intensity;

	// point light attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + (light.linear * distance) + light.quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result,1.0f);
}
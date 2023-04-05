#version 330 core
struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct Light{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPosition;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	vec3 viewDir = normalize(viewPosition - FragPos);

	// Ambient Light
	vec3 ambient = light.ambient * material.ambient;

	// Diffuse Light
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = (diff * material.diffuse) * light.diffuse;

	// Specular Light
	float specularStrength = 0.5f;
	vec3 reflectDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular = (spec * material.specular) * light.specular;

	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result,1.0f);
}
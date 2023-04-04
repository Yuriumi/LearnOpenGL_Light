#version 330 core
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPosition;
uniform vec3 viewPosition;

vec3 lightColor = vec3(1.0f);
vec3 objectColor = vec3(1.0f,0.5f,0.31f);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPosition - FragPos);
	vec3 viewDir = normalize(viewPosition - FragPos);

	// Ambient Light
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;

	// Diffuse Light
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diff * lightColor;

	// Specular Light
	float specularStrength = 0.5f;
	vec3 reflectDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular = spec * specularStrength * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;

	FragColor = vec4(result,1.0f);
}
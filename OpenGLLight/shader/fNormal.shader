#version 330 core
struct Material{
	// vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material material;

struct DirLight{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

struct PointLight{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform PointLight pointLight;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPosition;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir);

void main()
{
	// basic attribute
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPosition - FragPos);

	// add Light
	vec3 result;
	result += CalcDirLight(dirLight,norm,viewDir);
	result += CalcPointLight(pointLight,norm,FragPos,viewDir);

	FragColor = vec4(result,1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	// 我们需要的使片元指向光源的向量
	vec3 lightDir = normalize(-light.direction);
	// 漫反射着色
	float diff = max(dot(lightDir,normal),0.0);
	// 镜面光着色
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(viewDir,reflectDir),0.0f),material.shininess);
	// 合并结果
	vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// 漫反射着色
	float diff = max(dot(normal,lightDir),0.0f);
	// 镜面着色
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(viewDir, reflectDir),0.0f),material.shininess);
	// 光照衰减
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// 合并结果
	vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}
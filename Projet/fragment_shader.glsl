#version 330 core

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D diffuse; // Texture diffuse
    vec3 specular;
    float shininess;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord; // Coordonnées de texture reçues du vertex shader

out vec4 FragColor;

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main() {

    // Composante ambiante
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb + vec3(0.3f,0.3f,0.3f);

    // Composante diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;

    // Composante spéculaire
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}

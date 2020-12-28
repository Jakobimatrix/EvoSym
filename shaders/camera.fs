#version 130

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform Material material;
uniform sampler2D objectTexture;
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 cameraPos;
  
in vec3 FragNormal;
in vec3 VertexColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec4 FragPosLightSpace;

out vec4 FragColor;

void main()
{
    /// lightning
    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 frag_normal = normalize(FragNormal);  // TODO THIS SHOULD BE GIVEN!
    vec3 light_direction = normalize(light.position - FragPos);
    float diffuse_value = max(dot(frag_normal, light_direction), 0.0);
    vec3 diffuse = light.diffuse * (diffuse_value * material.diffuse);

    // specular
    vec3 view_direction = normalize(cameraPos - FragPos);
    vec3 reflect_direction = reflect(-light_direction, frag_normal);
    // vec3 reflect_direction = normalize(light_direction + view_direction);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);


    vec3 lighting = (ambient + diffuse + specular) * VertexColor;
    FragColor = vec4(lighting, 1.0);
}


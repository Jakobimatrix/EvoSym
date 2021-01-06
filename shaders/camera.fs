#version 130

struct Material {
    vec3 selfGlow;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 color;
};

uniform Light light;
uniform Material material;
uniform sampler2D objectTexture;
uniform sampler2D shadowMap;
uniform vec3 cameraPos;
  
in vec3 FragNormal;
in vec3 VertexColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec4 FragPosLightSpace;

out vec4 FragColor;

vec3 clamp3(vec3 v, float min, float max)
{
  v.r = clamp(v.r, min, max);
  v.g = clamp(v.g, min, max);
  v.b = clamp(v.b, min, max);
  return v;
}

void main()
{
    /// lightning

    // diffuse
    vec3 dirlight = light.direction;
    dirlight.x = -dirlight.x;
    float diffuse_value = max(dot(FragNormal, -dirlight), 0.0);
    vec3 diffuse = light.color * (diffuse_value * material.diffuse);

/*
    // specular
    vec3 view_direction = normalize(cameraPos - FragPos);
    vec3 reflect_direction = reflect(-light.direction, FragNormal);
    // vec3 reflect_direction = normalize(light.direction + view_direction);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular = light.color * (spec * material.specular);
*/

    vec3 lightning = light.ambient + diffuse;// + specular;

    lightning = clamp3(lightning,0,1);

    vec3 color = vec3(texture(objectTexture, TexCoord) * vec4(VertexColor, 1.0));

    vec3 color_frag = lightning*color + material.selfGlow*color;
    color_frag = clamp3(color_frag,0,1);
    FragColor = vec4(color_frag , 1.0);
}


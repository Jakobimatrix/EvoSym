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

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    /// lightning

    // diffuse
    float diffuse_value = max(dot(FragNormal, -light.direction), 0.0);
    vec3 diffuse = light.color * (diffuse_value * material.diffuse);


    // specular
    vec3 view_direction = normalize(FragPos - cameraPos);
    vec3 reflect_direction = reflect(light.direction, FragNormal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);

    // flashlight effect
    //reflect_direction = normalize(light.direction + view_direction);
    //float spec = spec + pow(max(dot(-view_direction, reflect_direction), 0.0), material.shininess);

    vec3 specular = light.color * (spec * material.specular);

    float shadow = ShadowCalculation(FragPosLightSpace);

    //vec3 lightning = light.ambient + diffuse;
    //vec3 lightning = light.ambient + specular;
    vec3 lightning = (light.ambient + diffuse + specular)*(1.0 - shadow);
    lightning = clamp3(lightning,0,1);

    vec3 color = vec3(texture(objectTexture, TexCoord) * vec4(VertexColor, 1.0));

    vec3 color_frag = lightning*color + material.selfGlow*color;
    color_frag = clamp3(color_frag,0,1);
    FragColor = vec4(color_frag , 1.0);
}


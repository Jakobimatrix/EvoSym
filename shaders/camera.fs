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
uniform sampler2D shadowBufferTexture;
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

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowBufferTexture, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // TODO branchless!!!!
    // regions outside light frustum
    if(projCoords.z > 1.0){
      return 0.0;
    }

    // shadow bias see: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
    float bias = max(0.00005 * (1.0 - dot(normal, lightDir)), 0.000005);

    //// 1 sample
    //// check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    // 9 samples PCF (percentage-closer filtering)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowBufferTexture, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowBufferTexture, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

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
    float angle_reflection = dot(view_direction, reflect_direction);
    float spec = pow(max(angle_reflection, 0.0), material.shininess);

    // flashlight effect
    //reflect_direction = normalize(light.direction + view_direction);
    //float spec = spec + pow(max(dot(-view_direction, reflect_direction), 0.0), material.shininess);

    //vec3 specular = light.color * (spec * material.specular);
    vec3 specular = vec3(spec,spec,spec);

    float shadow = ShadowCalculation(FragPosLightSpace, FragNormal, light.direction);

    //vec3 lightning = light.ambient + diffuse;
    //vec3 lightning = light.ambient + specular;
    //vec3 lightning = (light.ambient + diffuse + specular);
    vec3 lightning = (light.ambient + diffuse + specular)*(1.0 - shadow);
    lightning = clamp3(lightning,0,1);

    vec3 color = vec3(texture(objectTexture, TexCoord) * vec4(VertexColor, 1.0));

    vec3 color_frag = lightning*color + material.selfGlow*color;
    color_frag = clamp3(color_frag,0,1);
    FragColor = vec4(color_frag , 1.0);
}


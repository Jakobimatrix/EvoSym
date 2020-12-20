#version 130 

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 cameraPos;

out vec4 FragColor;
  
in vec3 FragNormal;
in vec3 VertexColor;
in vec2 TexCoord;
in vec3 FragPos;

float ambient_strength = 0.1; // TODO input?
vec3 light_source_color = vec3(1,1,1); // TODO input?
float specular_strength = 0.5; // TODO input?
float shininess = 32; // TODO input, must be power of 2, the higher the more diffuse is the reflection of the light source


void main()
{
    /// lightning
    // ambient
    vec3 ambient = ambient_strength * light_source_color;

    // diffuse
    vec3 frag_normal = normalize(FragNormal);     // TODO THIS SHOULD BE GIVEN!
    vec3 light_direction = normalize(lightPos - FragPos);
    float angle_rad = max(dot(frag_normal, light_direction), 0.0);
    vec3 diffuse = angle_rad * light_source_color;

    // specular
    vec3 view_direction = normalize(cameraPos - FragPos);
    vec3 reflect_direction = reflect(-light_direction, frag_normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), shininess);
    vec3 specular = specular_strength * spec * light_source_color;

    vec3 result_color = (ambient + diffuse + specular) * VertexColor;
    FragColor = vec4(result_color, 1.0);
}


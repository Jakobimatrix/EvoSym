#version 130


out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;

void main()
{
    float depthValue = texture(depthMap, TexCoords).r;
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic

    vec3 texture_color = vec3(texture(depthMap, TexCoords));
    FragColor = vec4(texture_color,1);

}

#version 130

uniform sampler2D depthMap;

in vec2 TexCoords;

out vec4 FragColor;


void main()
{
    // openGL
    float depthValue = texture(depthMap, TexCoords).r;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}

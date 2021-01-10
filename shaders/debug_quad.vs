#version 130

in highp vec3 vertexPos;
in mediump vec2 vertexTexturePos;

out vec2 TexCoords;

void main()
{
    TexCoords = vertexTexturePos;
    gl_Position = vec4(vertexPos, 1.0);
}

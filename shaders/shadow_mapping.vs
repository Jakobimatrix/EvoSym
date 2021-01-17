#version 130
in highp vec3 vertexPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 transformMesh2World;

void main()
{
    gl_Position = lightSpaceMatrix * transformMesh2World * vec4(vertexPos, 1.0);
}

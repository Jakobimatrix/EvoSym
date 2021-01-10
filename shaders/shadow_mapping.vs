#version 130
in highp vec3 vertexPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 transformMesh2World;


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
void main()
{
    gl_Position = lightSpaceMatrix * transformMesh2World * vec4(vertexPos, 1.0);

    //gl_Position = vec4(rand(vec2(vertexPos)),rand(vec2(vertexPos.yz)), 0, 1);
}

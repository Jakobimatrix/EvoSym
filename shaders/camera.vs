#version 130

in highp vec3 vertexPos;
in lowp vec3 vertexNormal;
in lowp vec3 vertexTangent;
in lowp vec3 vertexBitangent;
in mediump vec2 vertexTexturePos;
in mediump vec3 vertexColor;

// called model in diverse tutorials
uniform mat4 transformMesh2World;
uniform mat4 transformWorld2camera;
uniform mat4 projection;

out vec3 VertexColor;
out vec2 TexCoord;
out vec3 FragPos;
out vec3 FragNormal;


void main()
{
    // gl outs
    gl_Position = projection * transformWorld2camera * transformMesh2World * vec4(vertexPos, 1.0);

    // outs
    //FragNormal = vec3(transformMesh2World * vec4(vertexNormal, 1.0));
    mat3 rotation = mat3(transformMesh2World);
    FragNormal = rotation*vertexNormal;
    VertexColor = vertexColor;
    TexCoord = vertexTexturePos;
    FragPos = vec3(transformMesh2World * vec4(vertexPos, 1.0));

}


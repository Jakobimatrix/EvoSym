#version 130

in highp vec3 vertexPos;
in lowp vec3 vertexNormal;
in lowp vec3 vertexTangent;
in lowp vec3 vertexBitangent;
in mediump vec2 vertexTexturePos;
in mediump vec3 vertexColor;

// called model in diverse tutorials
uniform mat4 objectPoseTransformation;
uniform mat4 invCameraViewTransformation;
uniform mat4 projection;

out vec3 VertexColor;
out vec2 TexCoord;
out vec3 FragPos;
out vec3 FragNormal;


void main()
{
    // gl outs
    gl_Position = projection * invCameraViewTransformation * objectPoseTransformation * vec4(vertexPos, 1.0);

    // outs
    FragNormal = mat3(objectPoseTransformation) * vertexNormal;
    VertexColor = vertexColor;
    TexCoord = vertexTexturePos;
    FragPos = vec3(objectPoseTransformation * vec4(vertexPos, 1.0));

}


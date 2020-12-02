#version 130

in highp vec3 vertexPos;
in lowp vec3 vertexNormal;
in lowp vec3 vertexTangent;
in lowp vec3 vertexBitangent;
in mediump vec2 vertexTexturePos;
in mediump vec3 vertexColor;

uniform mat4 pose;
uniform mat4 view;
uniform mat4 projection;

out vec4 VertexColor;
out vec2 TexCoord;


void main()
{
    gl_Position = projection * view * pose * vec4(vertexPos, 1.0);

    float r,g,b;
    if(vertexPos.x > 0){
      r = 1;
    }else{
      r = 0;
    }

    if(vertexPos.y > 0){
      g = 1;
    }else{
      g = 0;
    }

    if(vertexPos.z > 0){
      b = 1;
    }else{
      b = 0;
    }
    VertexColor = vec4(vertexColor, 0.2);

    TexCoord = vertexTexturePos;

}


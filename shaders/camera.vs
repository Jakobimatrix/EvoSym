#version 130

in highp vec3 meshPos;
in lowp vec3 meshNormal;
in mediump vec2 meshTexture;
in lowp vec3 meshTangent;
in lowp vec3 meshBittangent;

uniform mat4 pose;
uniform mat4 view;
uniform mat4 projection;

out vec4 VertexColor;
out vec2 TexCoord;


void main()
{
    gl_Position = projection* view * pose *vec4(meshPos, 1.0);

    float r,g,b;
    if(meshPos.x > 0){
      r = 1;
    }else{
      r = 0;
    }

    if(meshPos.y > 0){
      g = 1;
    }else{
      g = 0;
    }

    if(meshPos.z > 0){
      b = 1;
    }else{
      b = 0;
    }
    VertexColor = vec4(r, g, b, 0.2); // set the output variable to a dark-red color

    TexCoord = meshTexture;

}


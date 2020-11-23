#version 130

in highp vec3 meshPos;
in lowp vec3 meshNormal;
in mediump vec2 meshTexture;
in lowp vec3 meshTangent;
in lowp vec3 meshBittangent;

uniform mat4 pose;
uniform mat4 view;

out vec4 vertexColor; // specify a color output to the fragment shader


void main()
{
// 1
  //gl_Position = view * pose * vec4(aPos, 1.0f);
  // gl_Position = vec4(meshPos, 1.0f);

// 2
    //// transform the vertex position
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    //// transform the texture coordinates
    //gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    //// forward the vertex color
    //gl_FrontColor = gl_Color;
// 3
    gl_Position = view * pose *vec4(meshPos, 1.0);
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color

}


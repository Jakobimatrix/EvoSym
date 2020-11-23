#version 130

// for location definition see display/mesh.h
layout (location=0) in vec3 meshPos;
layout (location=1) in vec3 meshNormal;
layout (location=2) in vec2 meshTexture;
layout (location=3) in vec3 meshTangent;
layout (location=4) in vec3 meshBittangent;

uniform mat4 pose;
uniform mat4 view;

out vec4 vertexColor; // specify a color output to the fragment shader


void main()
{
//
1
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
    gl_Position = vec4(meshPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color

}


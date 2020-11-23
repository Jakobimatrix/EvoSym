#version 300 es

// for location definition see display/mesh.h
layout (location=0) in vec3 meshPos;
layout (location=1) in vec3 meshNormal;
layout (location=2) in vec2 meshTexture;
layout (location=3) in vec3 meshTangent;
layout (location=4) in vec3 meshBittangent;

uniform mat4 pose;
uniform mat4 view;


void main()
{
  //gl_Position = view * pose * vec4(aPos, 1.0f);

  // gl_Position = vec4(meshPos, 1.0f);
}


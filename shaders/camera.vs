#version 130

in highp vec3 meshPos;
in lowp vec3 meshNormal;
in mediump vec2 meshTexture;
in lowp vec3 meshTangent;
in lowp vec3 meshBittangent;

uniform mat4 pose;
uniform mat4 view;
uniform mat4 projection;

out vec4 vertexColor; // specify a color output to the fragment shader


void main()
{
    gl_Position = projection* view * pose *vec4(meshPos, 1.0);
    vertexColor = vec4(0.5, 0.0, 0.0, 0.8); // set the output variable to a dark-red color

}


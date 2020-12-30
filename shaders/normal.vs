#version 130

in highp vec3 vertexPos;
in mediump vec3 vertexColor;

// called model in diverse tutorials
uniform mat4 objectPoseTransformation;
uniform mat4 invCameraViewTransformation;
uniform mat4 projection;

out vec3 VertexColor;

void main()
{
    // gl outs
    gl_Position = projection * invCameraViewTransformation * objectPoseTransformation * vec4(vertexPos, 1.0);

    // outs
    VertexColor = vertexColor;
}


#version 130

void main()
{
    // openGL
    gl_FragDepth = gl_FragCoord.z;
}

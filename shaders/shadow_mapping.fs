#version 130

out vec4 FragColor;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    gl_FragDepth = gl_FragCoord.z;
    gl_FragDepth = rand(vec2(gl_FragCoord));

    FragColor = vec4(rand(vec2(gl_FragCoord)),rand(vec2(gl_FragCoord)),rand(vec2(gl_FragCoord)),1);
}

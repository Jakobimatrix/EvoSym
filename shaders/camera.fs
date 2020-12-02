#version 130 

uniform sampler2D texture1;

out vec4 FragColor;
  
in vec4 VertexColor;
in vec2 TexCoord;


void main()
{
    //FragColor = texture(texture1, TexCoord) * VertexColor;
    FragColor = VertexColor;
}


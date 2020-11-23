#version 130 

//uniform sampler2D texture;

out vec4 FragColor;
  
in vec4 vertexColor;


void main()
{
    FragColor = vertexColor;
}


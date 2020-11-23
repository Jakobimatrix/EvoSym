#version 130 

//uniform sampler2D texture;

out vec4 FragColor;
  
in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  


void main()
{
    FragColor = vertexColor;
}


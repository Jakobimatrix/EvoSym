#include "worldMesh.h"

void WorldMesh::loadVertices() {
  const std::string path = Globals::getInstance().getAbsPath2Shaders();
  const std::string vs = path + "camera.vs";
  const std::string fs = path + "camera.fs";

  /*
  if (false && !loadShader(vs, fs)) {
    if (!sf::Shader::isAvailable()) {
      ERROR("Shader not supported");
    } else {
      F_ERROR(
          "Failed to load Shader. Error in Shader? Do the files exist? {%s, "
          "%s} ",
          vs.c_str(),
          fs.c_str());
    }
  }*/


  if (false && sf::Shader::isAvailable()) {
    if (!loadShader(vs, fs)) {
      F_ERROR(
          "Failed to load Shader. Error in Shader? Do the files exist? {%s, "
          "%s} ",
          vs.c_str(),
          fs.c_str());
    }
  } else {
    ERROR("Shader not supported");
  }

  std::vector<Vertex> verices_temp;
  // clang-format off
   verices_temp.emplace_back(Vertex({-0.9f, -0.9f, -0.9f,
                                0,0,1,
                                0,0,
                                1,0,0,
                                0,1,0}));
   verices_temp.emplace_back(Vertex({-0.9f,  0.9f, -0.9f,
                                0,0,1,
                                1,0,
                                1,0,0,
                                0,1,0}));
   verices_temp.emplace_back(Vertex({-0.9f, -0.9f,  0.9f,
                                0,0,1,
                                0,1,
                                1,0,0,
                                0,1,0}));
   verices_temp.emplace_back(Vertex({-0.9f, -0.9f,  0.9f,
                                0,0,1,
                                0,1,
                                1,0,0,
                                0,1,0}));
   verices_temp.emplace_back(Vertex({-0.9f,  0.9f, -0.9f,
                                0,0,1,
                                1,0,
                                1,0,0,
                                0,1,0}));
   verices_temp.emplace_back(Vertex({-0.9f,  0.9f,  0.9f,
                                0,0,1,
                                1,1,
                                1,0,0,
                                0,1,0}));


    //clang-format off
    std::vector<unsigned int> indices_temp;
    for(unsigned int i = 0; i < verices_temp.size(); i++){
      indices_temp.push_back(i);
    }
    std::string texture = Globals::getInstance().getAbsPath2Resources() + "save.png";
    init(verices_temp, indices_temp, texture);

    /*
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    */

}

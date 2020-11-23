#include "worldMesh.h"

void WorldMesh::loadVertices() {


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


    //clang-format on

    std::vector<unsigned int> indices_temp;
    for(unsigned int i = 0; i < verices_temp.size(); i++){
      indices_temp.push_back(i);
    }
    std::string texture = Globals::getInstance().getAbsPath2Resources() + "save.png";
    init(verices_temp, indices_temp, texture);

    // todo reuse vertice

    /*
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    */

}


void WorldMesh::loadShader(){
 const std::string path = Globals::getInstance().getAbsPath2Shaders();
  const std::string vs = path + "camera.vs";
  const std::string fs = path + "camera.fs";

  const bool load_shader_sf = false;
  const bool load_shader_self = true;

  if (sf::Shader::isAvailable()) {
    if (load_shader_self) {
      if (!Mesh::loadShader(vs, fs)) {
        F_ERROR(
            "Failed to load Shader. Error in Shader? Do the files exist? {%s, "
            "%s} ",
            vs.c_str(),
            fs.c_str());
      } else {
        DEBUG("Using Self made shader class.");
      }
    } else if (load_shader_sf) {
      if (!Mesh::loadShaderSf(vs, fs)) {
        F_ERROR(
            "Failed to load Shader. Error in Shader? Do the files exist? {%s, "
            "%s} ",
            vs.c_str(),
            fs.c_str());
      } else {
        DEBUG("Using sf::shader class.");
      }
    }
  } else {
    ERROR("Shader not supported");
  }

}

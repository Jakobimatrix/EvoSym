#include "arrowMesh.h"

void ArrowMesh::loadVertices() {

  std::vector<VertexType> verices_temp;
  verices_temp.reserve(6 * 6);
  // clang-format off

  std::array<float, 33> alg_vertex =
  {{1.f, -0.6f, -0.6f,
    1.f,  0.6f, -0.6f,
    1.f, -0.6f,  0.6f,
    1.f, -0.6f,  0.6f,
    1.f,  0.6f, -0.6f,
    1.f,  0.6f,  0.6f}};

  std::array<float, 12> alg_texture =
  {{0.f, 0.f,
    1.f, 0.f,
    0.f, 1.f,
    0.f, 1.f,
    1.f, 0.f,
    1.f,  1.f}};

   std::array<float, 6> vz_x = {{1,-1,1,1,1,1}};
   std::array<float, 6> vz_y = {{1,1,1,1,1,-1}};
   std::array<float, 6> vz_z = {{1,1,1,-1,1,1}};

   std::array<unsigned int, 6> x_offset = {{0,0,1,1,2,2}};
   std::array<unsigned int, 6> y_offset = {{1,1,2,2,0,0}};
   std::array<unsigned int, 6> z_offset = {{2,2,0,0,1,1}};

   for(unsigned int site = 0; site < 6; site++){
      for(unsigned int v = 0; v < 6; v++){
        float x = vz_x[site] * alg_vertex[v*3 + x_offset[site]];
        float y = vz_y[site] * alg_vertex[v*3 + y_offset[site]];
        float z = vz_z[site] * alg_vertex[v*3 + z_offset[site]];

        float text_x = alg_texture[v*2 + 0];
        float text_y = alg_texture[v*2 + 1];

        verices_temp.emplace_back(VertexType({
                                    x,y,z,
                                    0,1,0}));
      }
   }

    //clang-format on

    std::vector<unsigned int> indices_temp;
    indices_temp.reserve(verices_temp.size());
    for(unsigned int i = 0; i < verices_temp.size(); i++){
      indices_temp.push_back(i);
    }
    std::string texture = Globals::getInstance().getAbsPath2Resources() + "wall.jpg";
    init(verices_temp, indices_temp, texture);

    // todo reuse vertice

    /*
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    */

}


void ArrowMesh::loadShader(){
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

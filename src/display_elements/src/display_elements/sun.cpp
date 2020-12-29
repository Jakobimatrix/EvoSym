#include "sun.h"

void SunMesh::loadVertices() {
  std::vector<VertexType> verices_temp;
  verices_temp.reserve(6 * 6 * 2);
  std::vector<unsigned int> indices_temp;
  indices_temp.reserve(6 * 6 * 2);

  // clang-format off

  std::array<float, 33> alg_vertex =
  {{0.1f, -0.1f, -0.1f,
    0.1f,  0.1f, -0.1f,
    0.1f, -0.1f,  0.1f,
    0.1f, -0.1f,  0.1f, // == [2]
    0.1f,  0.1f, -0.1f, // == [1]
    0.1f,  0.1f,  0.1f}};

  std::array<float, 12> alg_texture =
  {{0.f, 0.f,
    1.f, 0.f,
    0.f, 1.f,
    0.f, 1.f, // == [2]
    1.f, 0.f, // == [1]
    1.f,  1.f}};

   std::array<float, 6> vz_x = {{1,-1,1,1,1,1}};
   std::array<float, 6> vz_y = {{1,1,1,1,1,-1}};
   std::array<float, 6> vz_z = {{1,1,1,-1,1,1}};

   std::array<unsigned int, 6> x_offset = {{0,0,1,1,2,2}};
   std::array<unsigned int, 6> y_offset = {{1,1,2,2,0,0}};
   std::array<unsigned int, 6> z_offset = {{2,2,0,0,1,1}};
  // clang-format on

  int index = 0;
  for (unsigned int inner = 0; inner < 2; inner++) {
    if (inner) {
      for (unsigned int i = 0; i < 6; i++) {
        // depth of the dice (inner walls)
        alg_vertex[3 * i] -= 0.003;
      }
    }
    for (unsigned int site = 0; site < 6; site++) {
      for (unsigned int v = 0; v < 6; v++) {
        if (3 == v) {
          indices_temp.emplace_back(index - 1);
          continue;
        }
        if (4 == v) {
          indices_temp.emplace_back(index - 2);
          continue;
        }

        float x = vz_x[site] * alg_vertex[v * 3 + x_offset[site]];
        float y = vz_y[site] * alg_vertex[v * 3 + y_offset[site]];
        float z = vz_z[site] * alg_vertex[v * 3 + z_offset[site]];

        float text_x = alg_texture[v * 2 + 0];
        float text_y = alg_texture[v * 2 + 1];

        Eigen::Vector3f normal(0, 0, 0);
        if (site == 0) {
          normal.x() = 1;
        }
        if (site == 1) {
          normal.x() = -1;
        }
        if (site == 2) {
          normal.z() = 1;
        }
        if (site == 3) {
          normal.z() = -1;
        }
        if (site == 4) {
          normal.y() = 1;
        }
        if (site == 5) {
          normal.y() = -1;
        }

        if (inner) {
          normal *= -1;
        }
        // clang-format off
          verices_temp.emplace_back(VertexType({
                                      x,y,z,
                                      normal.x(),normal.y(),normal.z(),
                                      //1,0,0,
                                      //0,1,0,
                                      text_x,text_y,
                                      1,1,1
                                      }));
        // clang-format on
        indices_temp.emplace_back(index++);
      }
    }
  }


  std::string texture =
      Globals::getInstance().getAbsPath2Resources() + "wall.jpg";
  init(verices_temp, indices_temp, texture);
}


void SunMesh::loadShader() {
  const std::string path = Globals::getInstance().getAbsPath2Shaders();
  const std::string vs = path + "camera.vs";
  const std::string fs = path + "camera.fs";

  if (!Mesh::loadShader(vs, fs)) {
    F_ERROR(
        "Failed to load Shader. Error in Shader? Do the files exist? {%s, "
        "%s} ",
        vs.c_str(),
        fs.c_str());
  }
}

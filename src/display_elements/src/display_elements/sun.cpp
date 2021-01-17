#include "sun.h"

#include <display_elements/basicShapes.hpp>

void SunMesh::loadVertices() {
  constexpr size_t resolution = 20;
  unsigned int num_triangles;
  unsigned int num_vertices;
  const float radius = 0.3f;
  const float length = 17.5f;

  getcoordXYZInformation(num_vertices, num_triangles, resolution);

  std::vector<VertexType> verices_temp;
  verices_temp.reserve(num_vertices);
  std::vector<unsigned int> indices_temp;
  indices_temp.reserve(num_triangles * 3);

  coordXYZ(
      verices_temp, indices_temp, radius, resolution, length, {1, 0.5, 0.5}, {0.5, 1, 0.5}, {1, 1, 1});

  init(verices_temp, indices_temp);
}


void SunMesh::loadShader() {
  const std::string path = Globals::getInstance().getAbsPath2Shaders();
  const std::string vs = path + "camera.vs";
  const std::string fs = path + "camera.fs";
  Mesh::loadShader(vs, fs);
}

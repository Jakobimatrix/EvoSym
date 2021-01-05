#include "sun.h"

#include <display_elements/basicShapes.hpp>

void SunMesh::loadVertices() {
  constexpr size_t resolution = 50;
  constexpr float arrow_diameter = 0.3f;
  constexpr float arrow_length = 1.f;
  unsigned int num_triangles;
  unsigned int num_vertices;

  getCylinderInformation(num_vertices, num_triangles, resolution);

  std::vector<VertexType> verices_temp;
  verices_temp.reserve(num_vertices);
  std::vector<unsigned int> indices_temp;
  indices_temp.reserve(num_triangles * 3);

  cylinder(verices_temp, indices_temp, arrow_diameter, resolution, arrow_length, Vector3f(1.f, 1.f, 1.f));

  init(verices_temp, indices_temp);
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

#ifndef WORLD_MESH
#define WORLD_MESH

#include <Eigen/Geometry>
#include <display_elements/mesh.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <globals/globals.hpp>
#include <globals/macros.hpp>
#include <utils/eigen_glm_conversation.hpp>


class WorldMesh : public Mesh<true, true, false, false, true, true, 3> {
 public:
  WorldMesh() {
    loadVertices();
    loadShader();
    setTexture();
  }

  void loadVertices();
  void loadShader();
};

#endif

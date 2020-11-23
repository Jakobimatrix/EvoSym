#ifndef WORLD_MESH
#define WORLD_MESH

#include <glm/gtc/type_ptr.hpp>
#include <globals/globals.hpp>
#include <globals/macros.hpp>

#include "mesh.h"


class WorldMesh : public Mesh {
 public:
  WorldMesh() { loadVertices(); }

  void setPose(const glm::mat4 &pose) {
    if (shader != nullptr) {
      shader->use();
      shader->setMat4("pose", pose);
    }
  }

  void setView(const glm::mat4 &view) {
    if (shader != nullptr) {
      shader->use();
      shader->setMat4("view", view);
    }

    /* if sf shader
    if (shader != nullptr) {
      const float *pSource = reinterpret_cast<const float *>(glm::value_ptr(view));
      // TODO better solution for conversation?
      sf::Glsl::Mat4 todo(pSource);
      shader->setUniform("view", todo);
    }*/
  }

  void loadVertices();
};

#endif

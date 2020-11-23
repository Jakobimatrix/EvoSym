#ifndef WORLD_MESH
#define WORLD_MESH

#include <glm/gtc/type_ptr.hpp>
#include <globals/globals.hpp>
#include <globals/macros.hpp>

#include "mesh.h"


class WorldMesh : public Mesh {
 public:
  WorldMesh() {
    loadVertices();
    loadShader();
  }

  void setPose(const glm::mat4 &pose) {
    if (shader != nullptr) {
      glCheck(shader->use());
      glCheck(shader->setMat4("pose", pose));
      glUseProgram(0);
    }

    if (shader_sf != nullptr) {
      const float *pSource = reinterpret_cast<const float *>(glm::value_ptr(pose));
      // TODO better solution for conversation?
      sf::Glsl::Mat4 todo(pSource);
      glCheck(sf::Shader::bind(shader_sf.get()));
      glCheck(shader_sf->setUniform("pose", todo));
      sf::Shader::bind(nullptr);
    }
  }

  void setView(const glm::mat4 &view) {
    if (shader != nullptr) {
      shader->use();
      shader->setMat4("view", view);
      glUseProgram(0);
    }

    if (shader_sf != nullptr) {
      const float *pSource = reinterpret_cast<const float *>(glm::value_ptr(view));
      // TODO better solution for conversation?
      sf::Glsl::Mat4 todo(pSource);
      glCheck(sf::Shader::bind(shader_sf.get()));
      glCheck(shader_sf->setUniform("view", todo));
      sf::Shader::bind(nullptr);
    }
  }

  void setProjection(const glm::mat4 &projection) {
    if (shader != nullptr) {
      shader->use();
      shader->setMat4("projection", projection);
      glUseProgram(0);
    }

    if (shader_sf != nullptr) {
      const float *pSource = reinterpret_cast<const float *>(glm::value_ptr(projection));
      // TODO better solution for conversation?
      sf::Glsl::Mat4 todo(pSource);
      glCheck(sf::Shader::bind(shader_sf.get()));
      glCheck(shader_sf->setUniform("projection", todo));
      sf::Shader::bind(nullptr);
    }
  }

  void setTExture() {
    // https://learnopengl.com/Model-Loading/Mesh?????
    if (shader != nullptr) {
      glCheck(shader->use());
      glUseProgram(0);
    }
  }

  void loadVertices();
  void loadShader();
};

#endif

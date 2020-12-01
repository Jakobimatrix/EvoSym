#ifndef WORLD_MESH
#define WORLD_MESH

#include <Eigen/Geometry>
#include <display_elements/mesh.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <globals/globals.hpp>
#include <globals/macros.hpp>
#include <utils/eigen_glm_conversation.hpp>


class WorldMesh : public Mesh<true, true, true, true, true, true, 3> {
 public:
  WorldMesh() {
    loadVertices();
    loadShader();
    setTexture();
  }

  void setPose(const Eigen::Affine3f &p) {
    Mesh::setPose(p);
    if (shader != nullptr) {
      const glm::mat4 pose_glm = utils::EigenAffine2GlmMat(this->pose);
      glCheck(shader->use());
      glCheck(shader->setMat4("pose", pose_glm));
      glUseProgram(0);
    }

    if (shader_sf != nullptr) {
      const float *pSource = reinterpret_cast<const float *>(this->pose.data());
      // TODO better solution for conversation?
      sf::Glsl::Mat4 pose_glsl(pSource);
      glCheck(sf::Shader::bind(shader_sf.get()));
      glCheck(shader_sf->setUniform("pose", pose_glsl));
      sf::Shader::bind(nullptr);
    }
  }

  void setView(const Eigen::Affine3d &view) {
    if (shader != nullptr) {
      const glm::mat4 view_glm = utils::EigenAffine2GlmMat(view);
      glCheck(shader->use());
      glCheck(shader->setMat4("view", view_glm));
      glUseProgram(0);
    }

    if (shader_sf != nullptr) {
      const float *pSource = reinterpret_cast<const float *>(view.data());
      // TODO better solution for conversation?
      sf::Glsl::Mat4 view_glsl(pSource);
      glCheck(sf::Shader::bind(shader_sf.get()));
      glCheck(shader_sf->setUniform("view", view_glsl));
      sf::Shader::bind(nullptr);
    }
  }

  void setProjection(const Eigen::Affine3d &projection) {
    if (shader != nullptr) {
      const glm::mat4 projection_glm = utils::EigenAffine2GlmMat(projection);
      glCheck(shader->use());
      glCheck(shader->setMat4("projection", projection_glm));
      glUseProgram(0);
    }

    if (shader_sf != nullptr) {
      const float *pSource = reinterpret_cast<const float *>(projection.data());
      // TODO better solution for conversation?
      sf::Glsl::Mat4 projection_glsl(pSource);
      glCheck(sf::Shader::bind(shader_sf.get()));
      glCheck(shader_sf->setUniform("projection", projection_glsl));
      sf::Shader::bind(nullptr);
    }
  }

  void setTexture() {
    if (shader != nullptr) {
      glCheck(shader->use());
      glCheck(shader->setInt("texture1", texture));
      glUseProgram(0);
    }

    if (shader_sf != nullptr) {
      ERROR("To lazy to implement for now");
    }
  }

  void loadVertices();
  void loadShader();
};

#endif

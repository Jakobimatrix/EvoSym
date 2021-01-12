// source: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h
// https://learnopengl.com/Getting-started/Shaders
#ifndef SHADER_H
#define SHADER_H

#include <Eigen/Geometry>
#include <QOpenGLShaderProgram>
#include <fstream>
#include <glm/glm.hpp>
#include <globals/macros.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utils/eigen_glm_conversation.hpp>

class Shader : public QOpenGLShaderProgram {
 public:
  Shader(QObject *parent = nullptr) : QOpenGLShaderProgram(parent) {}

  ~Shader() {}
  void use() { bind(); }

  void setBool(const char *name, bool value) const {
    setInt(name, static_cast<int>(value));
  }

  void setInt(const char *name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name), value);
  }

  void setFloat(const char *name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name), value);
  }

  void setVec2(const char *name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name), 1, &value[0]);
  }
  void setVec2(const char *name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name), x, y);
  }

  void setVec3(const char *name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
  }
  void setVec3(const char *name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
  }

  void setVec3(const char *name, const Eigen::Vector3f &value) const {
    glUniform3f(glGetUniformLocation(ID, name), value.x(), value.y(), value.z());
  }
  void setVec3(const char *name, const Eigen::Vector3d &value) const {
    const glm::vec3 value_glm = utils::EigenVec32GlmVec3(value);
    setVec3(name, value_glm);
  }

  void setVec4(const char *name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ID, name), 1, &value[0]);
  }
  void setVec4(const char *name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
  }

  void setMat2(const char *name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
  }

  void setMat3(const char *name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
  }

  void setMat4(const char *name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
  }
  template <typename T>
  void setMat4(const char *name, const Eigen::Matrix<T, 4, 4> &value) const {
    glm::mat4 value_glm;
    if constexpr (std::is_same<T, float>::value) {
      value_glm = utils::EigenMat42GlmMat4(value);
    } else {
      const Eigen::Matrix<float, 4, 4> valuef = value.template cast<float>();
      value_glm = utils::EigenMat42GlmMat4(valuef);
    }
    setMat4(name, value_glm);
  }

  GLint getUniformLocation(const char *name) const {
    auto ptr = uniform_locations.find(name);
    if (ptr != uniform_locations.end()) {
      return ptr->second;
    } else {
      const GLint loc = uniformLocation(name);
      uniform_locations[name] = loc;
      return loc;
    }
  }

  GLint getAttributeLocation(const char *name) const {
    auto ptr = attribute_locations.find(name);
    if (ptr != attribute_locations.end()) {
      return ptr->second;
    } else {
      const GLint loc = attributeLocation(name);
      attribute_locations[name] = loc;
      return loc;
    }
  }

 private:
  mutable std::map<std::string, GLint, std::less<>> uniform_locations;
  mutable std::map<std::string, GLint, std::less<>> attribute_locations;
};
#endif

#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <Eigen/Geometry>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <map>

#include "displayUtils.hpp"

QT_BEGIN_NAMESPACE

class ShaderProgram : public QOpenGLShaderProgram {
 public:
  ShaderProgram(QObject *parent = nullptr) : QOpenGLShaderProgram(parent) {}

  void use() { bind(); }

  void setBool(const char *name, bool value) const {
    setInt(name, static_cast<int>(value));
  }

  void setInt(const char *name, int value) const {
    QOpenGLFunctions *gl = QOpenGLContext::currentContext()->functions();
    glCheck(gl->glUniform1i(getUniformLocation(name), value));
  }

  void setFloat(const char *name, float value) const {
    QOpenGLFunctions *gl = QOpenGLContext::currentContext()->functions();
    glCheck(gl->glUniform1f(getUniformLocation(name), value));
  }

  template <typename T>
  void setVec3(const char *name, const Eigen::Matrix<T, 3, 1> &value) const {
    QOpenGLFunctions *gl = QOpenGLContext::currentContext()->functions();

    if constexpr (std::is_same<T, float>::value) {
      glCheck(gl->glUniform3fv(getUniformLocation(name), 1, value.data()));
    } else {
      const Eigen::Matrix<float, 3, 1> temp = value.template cast<float>();
      glCheck(gl->glUniform3fv(getUniformLocation(name), 1, temp.data()));
    }
  }

  template <typename T>
  void setMat4(const char *name, const Eigen::Matrix<T, 4, 4> &value) const {
    QOpenGLFunctions *gl = QOpenGLContext::currentContext()->functions();

    if constexpr (std::is_same<T, float>::value) {
      glCheck(gl->glUniformMatrix4fv(
          getUniformLocation(name), 1, GL_FALSE, value.data()));
    } else {
      const Eigen::Matrix<float, 4, 4> temp = value.template cast<float>();
      glCheck(gl->glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, temp.data()));
    }
  }

  GLint getUniformLocation(const char *name) const {
    auto ptr = uniform_locations.find(name);
    if (ptr != uniform_locations.end()) {
      return ptr->second;
    } else {
      const GLint loc = glCheck(uniformLocation(name));
      uniform_locations[name] = loc;
      return loc;
    }
  }

  GLint getAttributeLocation(const char *name) const {
    auto ptr = attribute_locations.find(name);
    if (ptr != attribute_locations.end()) {
      return ptr->second;
    } else {
      const GLint loc = glCheck(attributeLocation(name));
      attribute_locations[name] = loc;
      return loc;
    }
  }

 private:
  mutable std::map<std::string, GLint, std::less<>> uniform_locations;
  mutable std::map<std::string, GLint, std::less<>> attribute_locations;
};
#endif

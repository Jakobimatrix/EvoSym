#ifndef SHADOWS_HPP
#define SHADOWS_HPP

#include <stb/stb_image.h>

#include <Eigen/Geometry>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <globals/globals.hpp>
#include <memory>
#include <utils/eigen_conversations.hpp>

#include "displayUtils.hpp"
#include "shaderProgram.hpp"

// In case we want a light source emmitting in every direction:
// https://www.youtube.com/watch?v=vpDer0seP9M (Use depthCubeMap and GL_TEXTURE_CUBE_MAP)
class Shadows {
 public:
  Shadows(int size_x, int size_y, GLuint default_frame_buffer = 0) {
    setSize(size_x, size_y, default_frame_buffer);
  }

  ~Shadows() { clean(); }

  Shadows(GLuint default_frame_buffer = 0) {
    setSize(shadow_texture_width, shadow_texture_height, default_frame_buffer);
  }

  void setSize(int size_x, int size_y, GLuint default_frame_buffer = 0) {
    shadow_texture_width = size_x;
    shadow_texture_height = size_y;
    init(default_frame_buffer);
  }

  /*!
   * \brief Deletes the buffers.
   */
  void clean() {
    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();
    glCheck(gl->glDeleteFramebuffers(1, &depthMapFBO));
    glCheck(gl->glDeleteTextures(1, &depthMap));


    glCheck(gl->glDeleteVertexArrays(1, &quadVAO));
    glCheck(gl->glDeleteBuffers(1, &quadVBO));
    glCheck(gl->glDeleteBuffers(1, &quadEBO));
  }

  unsigned int getDepthMapTexture() { return depthMap; }

  bool is_bound = false;
  unsigned int getDepthMapFrameBufferInt() { return depthMapFBO; }

  void drawDebug() {
    if (shader_shadow_debug == nullptr) {
      return;
    }

    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();
    glCheck(shader_shadow_debug->use());
    glCheck(glActiveTexture(GL_TEXTURE0));

    glCheck(glBindTexture(GL_TEXTURE_2D, depthMap));
    glCheck(gl->glBindVertexArray(quadVAO));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glCheck(gl->glBindVertexArray(0));
    glCheck(gl->glUseProgram(0));
  }

 private:
  bool initiated = false;
  unsigned int depthMapFBO = 0;
  unsigned int depthMap = 0;
  int shadow_texture_width = 1024;
  int shadow_texture_height = 1024;

  std::shared_ptr<ShaderProgram> shader_shadow_debug = nullptr;
  unsigned int quadVAO = 0;
  unsigned int quadVBO = 0;
  unsigned int quadEBO = 0;

  // clang-format off
  float quadVertices[20] = {
      // positions        // texture Coords
      -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
      1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
  };
  // clang-format on

  unsigned int quadIndices[6] = {
      0,
      1,
      3,  // first triangle
      0,
      3,
      2  // second triangle
  };

  void init(GLuint default_frame_buffer = 0) {
    if (initiated) {
      clean();
    }
    QOpenGLFunctions *gl = QOpenGLContext::currentContext()->functions();


    // create depth texture
    glCheck(glGenTextures(1, &depthMap));
    if (depthMap == 0) {
      ERROR("Cant create texture. Do we have context???");
    }
    glCheck(glBindTexture(GL_TEXTURE_2D, depthMap));
    glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    // https://gamedev.stackexchange.com/questions/24000/fbo-depth-buffer-not-working
    glCheck(glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_texture_width, shadow_texture_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));

    // create frame buffer
    glCheck(gl->glGenFramebuffers(1, &depthMapFBO));
    if (depthMapFBO == 0) {
      ERROR("Cant create texture. Do we have context???");
    }
    // attach depth texture as FBO's depth buffer
    glCheck(gl->glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO));
    glCheck(gl->glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0));

    if (gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      ERROR("Framebuffer is not complete!");
    }

    initiated = true;
    prepareDebugShader();

    // clean up
    glCheck(gl->glBindFramebuffer(GL_FRAMEBUFFER, default_frame_buffer));
  }


  void prepareDebugShader() {
    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();
    const std::string path = Globals::getInstance().getAbsPath2Shaders();
    const std::string shadow_debug_vs = path + "debug_quad.vs";
    const std::string shadow_debug_fs = path + "debug_quad.fs";

    shader_shadow_debug = std::make_shared<ShaderProgram>();
    if (!shader_shadow_debug->addCacheableShaderFromSourceFile(
            QOpenGLShader::Vertex, QString::fromStdString(shadow_debug_vs))) {
      F_ASSERT("Failed to compile %s", shadow_debug_vs.c_str());
      return;
    }
    if (!shader_shadow_debug->addCacheableShaderFromSourceFile(
            QOpenGLShader::Fragment, QString::fromStdString(shadow_debug_fs))) {
      F_ASSERT("Failed to compile %s", shadow_debug_fs.c_str());
      return;
    }

    shader_shadow_debug->link();
    if (!shader_shadow_debug->isLinked()) {
      ASSERT("Failed to link Camera shader program.");
      return;
    }


    glCheck(shader_shadow_debug->use());
    glCheck(shader_shadow_debug->setInt("depthMap", 0));  // first texture is 0

    // setup plane VAO
    glCheck(gl->glGenVertexArrays(1, &quadVAO));
    glCheck(gl->glGenBuffers(1, &quadVBO));
    glCheck(gl->glGenBuffers(1, &quadEBO));

    if (quadVAO == 0 || quadVBO == 0 || quadEBO == 0) {
      ERROR("Failed to generate Buffers, Do we have Context????");
    }
    glCheck(gl->glBindVertexArray(quadVAO));
    glCheck(gl->glBindBuffer(GL_ARRAY_BUFFER, quadVBO));
    glCheck(gl->glBufferData(
        GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW));

    glCheck(gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO));
    glCheck(gl->glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW));

    disp_utils::assignShaderVariable(shader_shadow_debug->programId(),
                                     "vertexPos",
                                     3,
                                     5 * sizeof(float),
                                     reinterpret_cast<void *>(0 * sizeof(float)),
                                     gl);


    disp_utils::assignShaderVariable(shader_shadow_debug->programId(),
                                     "vertexTexturePos",
                                     2,
                                     5 * sizeof(float),
                                     reinterpret_cast<void *>(3 * sizeof(float)),
                                     gl);

    shader_shadow_debug->release();
  }
};

#endif

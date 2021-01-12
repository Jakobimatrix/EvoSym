#ifndef SHADOWS_HPP
#define SHADOWS_HPP

#include <stb/stb_image.h>

#include <Eigen/Geometry>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <globals/globals.hpp>
#include <memory>
#include <utils/eigen_conversations.hpp>

#include "displayUtils.hpp"
#include "shader.hpp"

// In case we want a light source emmitting in every direction:
// https://www.youtube.com/watch?v=vpDer0seP9M (Use depthCubeMap and GL_TEXTURE_CUBE_MAP)
class Shadows {
 public:
  Shadows(int size_x, int size_y) { setSize(size_x, size_y); }

  ~Shadows() { clean(); }

  Shadows() { setSize(shadow_texture_width, shadow_texture_height); }

  void setSize(int size_x, int size_y) {
    shadow_texture_width = size_x;
    shadow_texture_height = size_y;
    init();
  }

  /*!
   * \brief Deletes the buffers.
   */
  void clean() {
    glCheck(glDeleteFramebuffers(1, &depthMapFBO));
    glCheck(glDeleteTextures(1, &depthMap));


    glCheck(glDeleteVertexArrays(1, &quadVAO));
    glCheck(glDeleteBuffers(1, &quadVBO));
    glCheck(glDeleteBuffers(1, &quadEBO));
  }

  unsigned int getDepthMapTexture() { return depthMap; }

  unsigned int getDepthMapFrameBufferInt() { return depthMapFBO; }

  void drawDebug() {
    if (shader_shadow_debug == nullptr) {
      return;
    }
    glCheck(shader_shadow_debug->use());
    glCheck(glActiveTexture(GL_TEXTURE0));
    glCheck(glBindTexture(GL_TEXTURE_2D, depthMap));

    glCheck(glBindVertexArray(quadVAO));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glCheck(glBindVertexArray(0));
    glCheck(glUseProgram(0));
  }

 private:
  bool initiated = false;
  unsigned int depthMapFBO = 0;
  unsigned int depthMap = 0;
  int shadow_texture_width = 1024;
  int shadow_texture_height = 1024;

  std::shared_ptr<Shader> shader_shadow_debug = nullptr;
  unsigned int quadVAO = 0;
  unsigned int quadVBO = 0;
  unsigned int quadEBO = 0;

  float quadVertices[20] = {
      // positions        // texture Coords
      -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
  };

  unsigned int quadIndices[6] = {
      0,
      1,
      3,  // first triangle
      0,
      3,
      2  // second triangle
  };

  void init() {
    if (initiated) {
      clean();
    }

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
    glCheck(glGenFramebuffers(1, &depthMapFBO));
    if (depthMapFBO == 0) {
      ERROR("Cant create texture. Do we have context???");
    }
    // attach depth texture as FBO's depth buffer
    glCheck(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO));
    glCheck(glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      ERROR("Framebuffer is not complete!");
    }

    initiated = true;
    prepareDebugShader();

    // clean up
    glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  }


  void prepareDebugShader() {
    const std::string path = Globals::getInstance().getAbsPath2Shaders();
    const std::string shadow_debug_vs = path + "debug_quad.vs";
    const std::string shadow_debug_fs = path + "debug_quad.fs";
    shader_shadow_debug =
        std::make_shared<Shader>(shadow_debug_vs.c_str(), shadow_debug_fs.c_str());
    if (!shader_shadow_debug->isReady()) {
      shader_shadow_debug.reset();
      F_ERROR(
          "Failed to load shaddow Shader. Error in Shader? Do the files exist? "
          "{%s, "
          "%s} ",
          shadow_debug_vs.c_str(),
          shadow_debug_fs.c_str());
      return;
    }

    glCheck(shader_shadow_debug->use());
    glCheck(shader_shadow_debug->setInt("depthMap", 0));  // first texture is 0

    // setup plane VAO
    glCheck(glGenVertexArrays(1, &quadVAO));
    glCheck(glGenBuffers(1, &quadVBO));
    glCheck(glGenBuffers(1, &quadEBO));

    if (quadVAO == 0 || quadVBO == 0 || quadEBO == 0) {
      ERROR("Failed to generate Buffers, Do we have Context????");
    }
    glCheck(glBindVertexArray(quadVAO));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, quadVBO));
    glCheck(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW));

    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO));
    glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW));


    disp_utils::assignShaderVariable(shader_shadow_debug->ID,
                                     "vertexPos",
                                     3,
                                     5 * sizeof(float),
                                     reinterpret_cast<void*>(0 * sizeof(float)));


    disp_utils::assignShaderVariable(shader_shadow_debug->ID,
                                     "vertexTexturePos",
                                     2,
                                     5 * sizeof(float),
                                     reinterpret_cast<void*>(3 * sizeof(float)));

    glUseProgram(0);
  }
};

#endif

#ifndef MESH_H
#define MESH_H


#include <stb/stb_image.h>

#include <Eigen/Geometry>
#include <array>
#include <display_elements/displayUtils.hpp>
#include <display_elements/glad_import.hpp>
#include <display_elements/meshUtils.hpp>
#include <display_elements/shader.hpp>
#include <display_elements/vertex.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>
#include <utils/eigen_conversations.hpp>
#include <utils/eigen_glm_conversation.hpp>
#include <vector>

#include "light.hpp"
#include "material.hpp"


// for using "string"sv in constexpr map
using namespace std::literals::string_view_literals;

class BaseMesh {
 public:
  BaseMesh() {}
  virtual ~BaseMesh() {}
  virtual void draw() = 0;

  void setPose(const Eigen::Isometry3d& p) {
    pose = p;
    updatePose();
  }
  void translate(const Eigen::Vector3d& t) {
    pose.translate(t);
    updatePose();
  }

  void rotate(const Eigen::Vector3d& rpy) {
    pose.rotate(eigen_utils::rpy2RotationMatrix(rpy));
    updatePose();
  }

  void rotateAround(const Eigen::Vector3d& rpy, const Eigen::Vector3d& p) {
    Eigen::Vector3d diff = pose.translation() - p;
    pose.translate(-diff);
    pose.rotate(eigen_utils::rpy2RotationMatrix(rpy));
    pose.translate(diff);
  }

  void setView(const Eigen::Isometry3d& view) {
    if (shader != nullptr) {
      glCheck(shader->use());
      glCheck(shader->setMat4(SHADER_UNIFORM_VIEW_NAME, view.matrix()));
      glUseProgram(0);
    }
  }

  void setLight(const Light& light) {
    if (shader != nullptr) {
      glCheck(shader->use());
      glCheck(shader->setVec3(SHADER_UNIFORM_LIGHT_POSITION_NAME, light.getPosition()));
      glCheck(shader->setVec3(SHADER_UNIFORM_LIGHT_DIRECTION_NAME, light.getDirection()));
      glCheck(shader->setVec3(SHADER_UNIFORM_LIGHT_AMBIENT_NAME, light.getAmbient()));
      glCheck(shader->setVec3(SHADER_UNIFORM_LIGHT_COLOR_NAME, light.getColor()));
      glUseProgram(0);
    }
  }

  void setCameraPosition(const Eigen::Vector3d& pos) {
    if (shader != nullptr) {
      glCheck(shader->use());
      glCheck(shader->setVec3(SHADER_UNIFORM_CAMERA_POSITION_NAME, pos));
      glUseProgram(0);
    }
  }

  void setProjection(const Eigen::Projective3d& projection) {
    if (shader != nullptr) {
      glCheck(shader->use());
      glCheck(shader->setMat4(SHADER_UNIFORM_PROJECTION_NAME, projection.matrix()));
      glUseProgram(0);
    }
  }

  void setTexture(int sample_nr = 0) {
    if (shader != nullptr) {
      glCheck(shader->use());
      glCheck(shader->setInt("texture1", sample_nr));
      glUseProgram(0);
    }
  }

 protected:
  void setMaterial(const Material& material) {
    this->material = material;
    if (shader != nullptr) {
      glCheck(shader->use());
      glCheck(shader->setVec3(SHADER_UNIFORM_MATERIAL_SELFGLOW_NAME, material.self_glow));
      glCheck(shader->setVec3(SHADER_UNIFORM_MATERIAL_DIFFUSE_NAME, material.diffuse));
      glCheck(shader->setVec3(SHADER_UNIFORM_MATERIAL_SPECULAR_NAME, material.specular));
      glCheck(shader->setFloat(SHADER_UNIFORM_MATERIAL_SHININESS_NAME, material.shininess));
      glUseProgram(0);
    }
  }

  /*!
   * \brief Loads and connects a new shader to this mesh.
   * This uses the own shader class. An existing shader will be deleted.
   * \param vertex_shader_file The path to the vertex shader file.
   * \param fragment_shader_file The path to the vertex shader file.
   */
  [[nodiscard]] bool loadShader(const std::string& vertex_shader_file,
                                const std::string& fragment_shader_file) {
    shader = std::make_shared<Shader>(vertex_shader_file.c_str(),
                                      fragment_shader_file.c_str());
    if (!shader->isReady()) {
      shader.reset();
      return false;
    }
    connectShader(shader->ID);
    return true;
  }

  /*!
   * \brief Load texture and talk to openGL to request some space for the
   * texture.
   * \param texture_path The path to the texture/image.
   */
  void loadTexture(const std::string& texture_path) {
    glCheck(glGenTextures(1, &texture));

    glCheck(glBindTexture(GL_TEXTURE_2D, texture));  // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;

    unsigned char* data =
        stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);
    if (data == nullptr) {
      F_ERROR("Failed to load textre from %s.", texture_path.c_str());
      return;
    }

    glCheck(glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
    // glCheck(glGenerateMipmap(GL_TEXTURE_2D));

    // set the texture wrapping parameters
    glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));  // set texture wrapping to GL_REPEAT (default wrapping method)
    glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    // set texture filtering parameters
    glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    stbi_image_free(data);
  }

  /*!
   * \brief Deletes the buffers.
   */
  void clean() {
    glCheck(glDeleteVertexArrays(1, &VAO));
    glCheck(glDeleteBuffers(1, &VBO));
    glCheck(glDeleteBuffers(1, &EBO));
    glCheck(glDeleteTextures(1, &texture));
    is_initialized = false;
  }

  // render data
  unsigned int VBO, EBO;

  std::shared_ptr<Shader> shader = nullptr;

  virtual void connectShader(unsigned int shaderProgram) = 0;

  // mesh Data
  unsigned int texture;
  unsigned int VAO;

  // access in shader like this:
  // in vec3 vertexPos;
  static constexpr const char* SHADER_IN_POSITION_NAME = "vertexPos";
  static constexpr const char* SHADER_IN_NORMAL_NAME = "vertexNormal";
  static constexpr const char* SHADER_IN_TANGENT_NAME = "vertexTangent";
  static constexpr const char* SHADER_IN_BITANGENT_NAME = "vertexBitangent";
  static constexpr const char* SHADER_IN_TEXTURE_NAME = "vertexTexturePos";
  static constexpr const char* SHADER_IN_COLOR_NAME = "vertexColor";

  // shader standard uniform
  static constexpr const char* SHADER_UNIFORM_POSE_NAME =
      "objectPoseTransformation";
  static constexpr const char* SHADER_UNIFORM_VIEW_NAME =
      "invCameraViewTransformation";
  static constexpr const char* SHADER_UNIFORM_CAMERA_POSITION_NAME =
      "cameraPos";
  static constexpr const char* SHADER_UNIFORM_PROJECTION_NAME = "projection";
  static constexpr const char* SHADER_UNIFORM_CAMERA_OBJECT_TEXTURE_NAME =
      "objectTexture";
  static constexpr int SHADER_UNIFORM_CAMERA_OBJECT_TEXTURE_ID = 0;
  static constexpr const char* SHADER_UNIFORM_CAMERA_SHADOW_TEXTURE_NAME =
      "shadowTexture";
  static constexpr int SHADER_UNIFORM_CAMERA_SHADOW_TEXTURE_ID = 1;
  static constexpr const char* SHADER_UNIFORM_SHADOW_TEXTURE_NAME =
      "shadowBufferTexture";
  static constexpr int SHADER_UNIFORM_SHADOW_TEXTURE_ID = 0;
  static constexpr const char* SHADER_UNIFORM_LIGHT_POSITION_NAME =
      "light.position";
  static constexpr const char* SHADER_UNIFORM_LIGHT_DIRECTION_NAME =
      "light.direction";
  static constexpr const char* SHADER_UNIFORM_LIGHT_AMBIENT_NAME =
      "light.ambient";
  static constexpr const char* SHADER_UNIFORM_LIGHT_COLOR_NAME = "light.color";
  static constexpr const char* SHADER_UNIFORM_MATERIAL_SELFGLOW_NAME =
      "material.selfGlow";
  static constexpr const char* SHADER_UNIFORM_MATERIAL_DIFFUSE_NAME =
      "material.diffuse";
  static constexpr const char* SHADER_UNIFORM_MATERIAL_SPECULAR_NAME =
      "material.specular";
  static constexpr const char* SHADER_UNIFORM_MATERIAL_SHININESS_NAME =
      "material.shininess";


  bool is_initialized = false;
  Eigen::Isometry3d pose = Eigen::Isometry3d::Identity();
  Material material;

 private:
  void updatePose() {
    if (shader != nullptr) {
      glCheck(shader->use());
      glCheck(shader->setMat4(SHADER_UNIFORM_POSE_NAME, pose.matrix()));
      glUseProgram(0);
    }
  }
};

template <bool has_position = true, bool has_normal = true, bool has_tangent = true, bool has_bitangent = true, bool has_texture = true, bool has_color = true, int num_color_values = 3>
class Mesh : public BaseMesh {
 public:
  using VertexType =
      Vertex<has_position, has_normal, has_tangent, has_bitangent, has_texture, has_color, num_color_values>;

  Mesh() : BaseMesh() {}

  ~Mesh() { clean(); }

  /*!
   * \brief Reserves space on GPU for texture and vertices
   * \param vertices The vector of vertices describeing the mesh and what not.
   * \param indices A vector of indices describeing the order of the vertices.
   * \param texture_path The path to the texture.
   */
  void init(std::vector<VertexType> vertices,
            std::vector<unsigned int> indices,
            const std::string& texture_path) {

    if (is_initialized) {
      clean();
    }
    this->vertices = vertices;
    this->indices = indices;

    loadTexture(texture_path);
    setupMesh();
    is_initialized = true;
  }

  /*!
   * \brief This renders the mesh using the active shader if set.
   */
  void draw() override {

    // TODO deal with uninitialized material, light, etc

    if (!is_initialized) {
      return;
    }

    glCheck(glBindTexture(GL_TEXTURE_2D, texture));
    if (shader != nullptr) {
      glCheck(shader->use());
    }

    // draw mesh
    glCheck(glBindVertexArray(VAO));
    glCheck(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr));
    glCheck(glBindVertexArray(0));

    if (shader != nullptr) {
      glCheck(glUseProgram(0));
    }

    // always good practice to set everything back to defaults once configured.
    glCheck(glActiveTexture(GL_TEXTURE0));
  }

 protected:
  // mesh Data
  std::vector<VertexType> vertices;
  std::vector<unsigned int> indices;

 private:
  /*!
   * \brief Talks to openGL to reserve space for the mesh.
   */
  void setupMesh() {
    // create buffers/arrays
    glCheck(glGenVertexArrays(1, &VAO));
    glCheck(glGenBuffers(1, &VBO));
    glCheck(glGenBuffers(1, &EBO));

    glCheck(glBindVertexArray(VAO));
    // load data into vertex buffers
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glCheck(glBufferData(
        GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexType), &vertices[0], GL_STATIC_DRAW));

    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    glCheck(glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
  }

  /*!
   * \brief Connects the vertex shader input variables with the vertex array.
   * The input variables expected in the shader are:
   * in vec3 meshPos;
   * in vec3 meshNormal;
   * in vec2 meshTexture;
   * in vec3 meshTangent;
   * in vec3 meshBittangent;
   * This is kinda hard coded for now.
   */
  void connectShader(unsigned int shaderProgram) override {

    // Rather then using the recomended glBindAttribLocation prior to linking the
    // shader I did that here, which is bad according to some. but it works for
    // sfml shaders too which are linked somewhere deep in sfml.
    // also since I am stuck with #version 130 (GLSL 1.30)
    // I cannot use layout(location = 0) which is avaiable in GLSL 1.40
    auto assignShaderVariable = [&shaderProgram](const char* var_name,
                                                 int num_values,
                                                 void* start_position) {
      const int variable_position = glGetAttribLocation(shaderProgram, var_name);
      glCheckAfter();
      if (variable_position < 0) {
        // compiler did optimize away the variable
        F_WARNING(
            "Trying to connect to shader variable %s failed. Variable not "
            "found",
            var_name);
        return;
      }
      const unsigned int u_pos = static_cast<unsigned int>(variable_position);
      glCheck(glEnableVertexAttribArray(u_pos));
      glCheck(glVertexAttribPointer(
          u_pos, num_values, GL_FLOAT, GL_FALSE, sizeof(VertexType), start_position));

      F_DEBUG(
          "connecting %s with %d values starting at %p. Connected Position is "
          "%d",
          var_name,
          num_values,
          start_position,
          u_pos);
    };

    // set the vertex attribute pointers
    // vertex positions
    if constexpr (has_position) {
      assignShaderVariable(SHADER_IN_POSITION_NAME,
                           VertexType::NUM_POSITION,
                           reinterpret_cast<void*>(offsetof(VertexType, position)));
    }

    // vertex normals
    if constexpr (has_normal) {
      assignShaderVariable(SHADER_IN_NORMAL_NAME,
                           VertexType::NUM_NORMAL,
                           reinterpret_cast<void*>(offsetof(VertexType, normal)));
    }

    // vertex tangent
    if constexpr (has_tangent) {
      assignShaderVariable(SHADER_IN_TANGENT_NAME,
                           VertexType::NUM_TANGENT,
                           reinterpret_cast<void*>(offsetof(VertexType, tangent)));
    }

    // vertex bitangent
    if constexpr (has_bitangent) {
      assignShaderVariable(SHADER_IN_BITANGENT_NAME,
                           VertexType::NUM_BITANGENT,
                           reinterpret_cast<void*>(offsetof(VertexType, bitangent)));
    }

    // vertex texture coords
    if constexpr (has_texture) {
      assignShaderVariable(SHADER_IN_TEXTURE_NAME,
                           VertexType::NUM_TEXTURE,
                           reinterpret_cast<void*>(offsetof(VertexType, texture_pos)));
    }

    // vertex color coords
    if constexpr (has_color) {
      assignShaderVariable(SHADER_IN_COLOR_NAME,
                           VertexType::NUM_COLOR,
                           reinterpret_cast<void*>(offsetof(VertexType, color)));
    }
  }
};

#endif

#ifndef MESH_H
#define MESH_H


#include <SFML/Graphics/Shader.hpp>
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
#include <vector>
#include <utils/functions.h>

template <bool has_position = true, bool has_normal = true, bool has_tangent = true, bool has_bitangent = true, bool has_texture = true, bool has_color = true, int num_color_values = 3>
class Mesh {

  using VertexType =
      Vertex<has_position, has_normal, has_tangent, has_bitangent, has_texture, has_color, num_color_values>;

 public:
  // mesh Data
  std::vector<VertexType> vertices;
  std::vector<unsigned int> indices;
  unsigned int texture;
  unsigned int VAO;

  bool is_initialized = false;

  Mesh() {}

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
      glCheck(glDeleteVertexArrays(1, &VAO));
      glCheck(glDeleteBuffers(1, &VBO));
      glCheck(glDeleteBuffers(1, &EBO));
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
  void draw() {
    if (!is_initialized) {
      return;
    }

    glCheck(glBindTexture(GL_TEXTURE_2D, texture));
    if (shader != nullptr) {
      glCheck(shader->use());
    }
    if (shader_sf != nullptr) {
      glCheck(sf::Shader::bind(shader_sf.get()));
    }

    // draw mesh
    glCheck(glBindVertexArray(VAO));
    glCheck(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr));
    glCheck(glBindVertexArray(0));

    if (shader != nullptr) {
      glCheck(glUseProgram(0));
    }
    if (shader_sf != nullptr) {
      glCheck(sf::Shader::bind(nullptr));
    }

    // always good practice to set everything back to defaults once configured.
    glCheck(glActiveTexture(GL_TEXTURE0));
  }


  /*!
   * \brief Loads and connects a new shader to this mesh.
   * This uses the s::shader class. An existing shader will be deleted.
   * \param vertex_shader_file The path to the vertex shader file.
   * \param fragment_shader_file The path to the vertex shader file.
   */
  [[nodiscard]] bool loadShaderSf(const std::string& vertex_shader_file,
                                  const std::string& fragment_shader_file) {
    shader_sf = std::make_shared<sf::Shader>();
    const bool success = shader_sf->loadFromFile(vertex_shader_file, fragment_shader_file);
    if (!success) {
      shader_sf.reset();
    }
    shader.reset();
    connectShader(shader_sf->getNativeHandle());
    return success;
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
    shader_sf.reset();
    connectShader(shader->ID);
    return true;
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
  void connectShader(unsigned int shaderProgram) {

    // Rather then using the recomended glBindAttribLocation prior to linking the
    // shader I did that here, which is bad according to some. but it works for
    // sfml shaders too which are linked somewhere deep in sfml.
    // also since I am stuck with #version 130 (GLSL 1.30)
    // I cannot use layout(location = 0) which is avaiable in GLSL 1.40
    auto assignShaderVariable = [&shaderProgram](const std::string& var_name,
                                                 int num_values,
                                                 void* start_position) {
      const int variable_position = glGetAttribLocation(shaderProgram, var_name.c_str());
      if (variable_position < 0) {
        // compiler did optimize away the variable
        F_WARNING(
            "Trying to connect to shader variable %s failed. Variable not "
            "found",
            var_name.c_str());
        return;
      }
      // Check errors in case the compiler did not optimize away
      // but rather I did somthing wrong here...
      glCheck();
      const unsigned int u_pos = static_cast<unsigned int>(variable_position);
      glCheck(glEnableVertexAttribArray(u_pos));
      glCheck(glVertexAttribPointer(
          u_pos, num_values, GL_FLOAT, GL_FALSE, sizeof(VertexType), start_position));
    };

    // set the vertex attribute pointers
    // vertex Positions

    // TODO
    assignShaderVariable("meshPos", COORDS_PER_VERTEX, reinterpret_cast<void*>(0));

    // vertex normals
    assignShaderVariable("meshNormal",
                         COORDS_PER_VERTEX,
                         reinterpret_cast<void*>(offsetof(VertexType, Normal)));

    // vertex texture coords
    assignShaderVariable("meshTexture",
                         COORDS_PER_TEXTURE,
                         reinterpret_cast<void*>(offsetof(VertexType, TexCoords)));

    // vertex tangent
    assignShaderVariable("meshTangent",
                         COORDS_PER_VERTEX,
                         reinterpret_cast<void*>(offsetof(VertexType, Tangent)));

    // vertex bitangent
    assignShaderVariable("meshBittangent",
                         COORDS_PER_VERTEX,
                         reinterpret_cast<void*>(offsetof(VertexType, Bitangent)));
  }


 protected:
  std::shared_ptr<Shader> shader = nullptr;
  std::shared_ptr<sf::Shader> shader_sf = nullptr;

 private:
  /*!
   * \brief Load texture and talk to openGL to request some space for the
   * texture.
   * \param texture_path The path to the texture/image.
   */
  void loadTexture(const std::string& texture_path) {
    glCheck(glGenTextures(1, &texture));

    glCheck(glBindTexture(GL_TEXTURE_2D, texture));  // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;

    unsigned char* data =
        utils::loadTexture(texture_path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glCheck(glTexImage2D(
          GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
      glCheck(glGenerateMipmap(GL_TEXTURE_2D));
    } else {
      F_ERROR("Failed to load textre from %s.", texture_path.c_str());
    }
    utils::freeTexture(data);
  }

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

  // render data
  unsigned int VBO, EBO;

  unsigned int POSITION_LOCATION = 0;
  unsigned int NORMAL_LOCATION = 1;
  unsigned int TEXTURE_LOCATION = 2;
  unsigned int TANGENT_LOCATION = 3;
  unsigned int BITTANGENT_LOCATION = 4;

  int COORDS_PER_VERTEX = 3;
  int COORDS_PER_TEXTURE = 2;
};

#endif

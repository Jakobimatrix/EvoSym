#ifndef MESH_H
#define MESH_H

#include <warning_guards/warning_guards.h>

#include <SFML/Graphics/Shader.hpp>

THIRD_PARTY_HEADERS_BEGIN
#include <glad/glad.h>  // holds all OpenGL type declarations
THIRD_PARTY_HEADERS_END

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>
#include <vector>

#include <display_elements/shader.hpp>

struct Vertex {
  // position
  Vertex() {}
  Vertex(const std::array<float, 14>& v) {
    Position = glm::vec3(v[0], v[1], v[2]);
    Normal = glm::vec3(v[3], v[4], v[5]);
    TexCoords = glm::vec2(v[6], v[7]);
    Tangent = glm::vec3(v[8], v[9], v[10]);
    Bitangent = glm::vec3(v[11], v[12], v[13]);
  }
  Vertex(const glm::vec3& p,
         const glm::vec3& n,
         const glm::vec2& tex,
         const glm::vec3& t,
         const glm::vec3& b) {
    Position = p;
    Normal = n;
    TexCoords = tex;
    Tangent = t;
    Bitangent = b;
  }
  glm::vec3 Position;
  // normal
  glm::vec3 Normal;
  // texCoords
  glm::vec2 TexCoords;
  // tangent
  glm::vec3 Tangent;
  // bitangent
  glm::vec3 Bitangent;
};

class Mesh {
 public:
  // mesh Data
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  unsigned int texture;
  unsigned int VAO;

  bool is_initialized = false;

  Mesh() {}

  void init(std::vector<Vertex> vertices,
            std::vector<unsigned int> indices,
            const std::string& texture_path);

  // render the mesh
  void draw() ;

  /*
  [[nodiscard]] bool setShaderSf(std::shared_ptr<sf::Shader>& ptr) {
    shader_sf = ptr;
    return true;
  }


  [[nodiscard]] bool loadShaderSf(const std::string& vertex_shader_file,
                                  const std::string& fragment_shader_file) {
    shader_sf = std::make_shared<sf::Shader>();
    const bool success = shader_sf->loadFromFile(vertex_shader_file,
  fragment_shader_file); if (!success) { shader_sf.reset();
    }
    return success;
  }

  [[nodiscard]] bool loadVertexShader(const std::string& vertex_shader_file) {
    shader_sf = std::make_shared<sf::Shader>();
    const bool success = shader_sf->loadFromFile(vertex_shader_file,
  sf::Shader::Vertex); if (!success) { shader_sf.reset();
    }
    return success;
  }

  [[nodiscard]] bool loadFragmentShader(const std::string& fragment_shader_file)
  { shader_sf = std::make_shared<sf::Shader>(); const bool success = shader_sf->loadFromFile(fragment_shader_file,
  sf::Shader::Fragment); if (!success) { shader_sf.reset();
    }
    return success;
  }
  */

  [[nodiscard]] bool setShader(std::shared_ptr<Shader>& ptr) {
    shader = ptr;
    return true;
  }

  [[nodiscard]] bool loadShader(const std::string& vertex_shader_file,
                                const std::string& fragment_shader_file) {
    shader = std::make_shared<Shader>(vertex_shader_file.c_str(),
                                      fragment_shader_file.c_str());
    if (!shader->isReady()) {
      shader.reset();
      return false;
    }
    return true;
  }

 protected:
  std::shared_ptr<Shader> shader = nullptr;
  // std::shared_ptr<sf::Shader> shader = nullptr;

 private:
  // render data
  unsigned int VBO, EBO;

  unsigned int POSITION_LOCATION = 0;
  unsigned int NORMAL_LOCATION = 1;
  unsigned int TEXTURE_LOCATION = 2;
  unsigned int TANGENT_LOCATION = 3;
  unsigned int BITTANGENT_LOCATION = 4;

  int COORDS_PER_VERTEX = 3;
  int COORDS_PER_TEXTURE = 2;

  void loadTexture(const std::string& texture_path);

  // initializes all the buffer objects/arrays
  void setupMesh();

  void checkError(const std::string& operation);
};

#endif

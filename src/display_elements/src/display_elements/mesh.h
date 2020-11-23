#ifndef MESH_H
#define MESH_H

#include <warning_guards/warning_guards.h>

#include <SFML/Graphics/Shader.hpp>

THIRD_PARTY_HEADERS_BEGIN
#include <glad/glad.h>  // holds all OpenGL type declarations
THIRD_PARTY_HEADERS_END

#include <array>
#include <display_elements/shader.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>
#include <vector>

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

  /*!
   * \brief Reserves space on GPU for texture and vertices
   * \param vertices The vector of vertices describeing the mesh and what not.
   * \param indices A vector of indices describeing the order of the vertices.
   * \param texture_path The path to the texture.
   */
  void init(std::vector<Vertex> vertices,
            std::vector<unsigned int> indices,
            const std::string& texture_path);

  /*!
   * \brief This renders the mesh using the active shader if set.
   */
  void draw();

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
  void connectShader(unsigned int shaderProgram);

  /*!
   * \brief This prints out the last GL errors.
   * \param file The file name (use __FILE__)
   * \param line The line (use __LINE__)
   * \param expression The function call to GL
   * usage: glCheck(FUN2GL(val1, val2...));
   */
  static void checkError(const char* file, unsigned int line, const char* expression);


 protected:
  std::shared_ptr<Shader> shader = nullptr;
  std::shared_ptr<sf::Shader> shader_sf = nullptr;

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

  /*!
   * \brief Load texture and talk to openGL to request some space for the
   * texture.
   * \param texture_path The path to the texture/image.
   */
  void loadTexture(const std::string& texture_path);

  /*!
   * \brief Talks to openGL to reserve space for the mesh.
   */
  void setupMesh();
};

#ifdef NDEBUG
#define glCheck(expr) (expr)
#else
#define glCheck(expr) \
  expr;               \
  Mesh::checkError(__FILE__, __LINE__, #expr);
#endif

#endif

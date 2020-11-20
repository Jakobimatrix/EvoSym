#ifndef MESH
#define MESH

#include <warning_guards/warning_guards.h>

THIRD_PARTY_HEADERS_BEGIN
#include <glad/glad.h>  // holds all OpenGL type declarations
THIRD_PARTY_HEADERS_END

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

#include "shader.hpp"

struct Vertex {
  // position
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

struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

class Mesh {
 public:
  // mesh Data
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  unsigned int VAO;

  // constructor
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
  }

  // render the mesh
  void Draw(Shader& shader) {
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++) {
      glActiveTexture(GL_TEXTURE0 + i);  // active proper texture unit before binding
      // retrieve texture number (the N in diffuse_textureN)
      std::string number;
      std::string name = textures[i].type;
      if (name == "texture_diffuse")
        number = std::to_string(diffuseNr++);
      else if (name == "texture_specular")
        number = std::to_string(specularNr++);  // transfer unsigned int to stream
      else if (name == "texture_normal")
        number = std::to_string(normalNr++);  // transfer unsigned int to stream
      else if (name == "texture_height")
        number = std::to_string(heightNr++);  // transfer unsigned int to stream

      // now set the sampler to the correct texture unit
      glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
      // and finally bind the texture
      glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
  }

 private:
  // render data
  unsigned int VBO, EBO;

  // initializes all the buffer objects/arrays
  void setupMesh() {
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Tangent)));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
        4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Bitangent)));

    glBindVertexArray(0);
  }
};


class SimpleMesh {
 public:
  SimpleMesh() {}

  void draw() {
    // https://stackoverflow.com/questions/21652546/what-is-the-role-of-glbindvertexarrays-vs-glbindbuffer-and-what-is-their-relatio
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glNormalPointer(GL_FLOAT, COORDS_PER_VERTEX * sizeof(GLfloat), normal_buffer.data());
    glVertexPointer(COORDS_PER_VERTEX, GL_FLOAT, VERTEX_STRIDE, vertex_buffer.data());
    glColorPointer(NUM_COLOR_PIGMENTS, GL_FLOAT, COLOR_STRIDE, color_buffer.data());

    // draw mesh
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(index_buffer.size()));
    glBindVertexArray(0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
  }

 protected:
  std::vector<GLfloat> vertex_buffer;
  std::vector<GLfloat> normal_buffer;
  std::vector<GLfloat> index_buffer;
  std::vector<GLfloat> color_buffer;

  const static int COORDS_PER_VERTEX = 3;   // x,y,z
  const static int NUM_COLOR_PIGMENTS = 4;  // r,g,b,a
  const static int VERTEX_STRIDE = 0;
  const static int COLOR_STRIDE = 0;
};



#endif

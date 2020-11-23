#include "mesh.h"

#include <warning_guards/warning_guards.h>

THIRD_PARTY_HEADERS_BEGIN
#include <stb/stb_image.h>
THIRD_PARTY_HEADERS_END

void Mesh::init(std::vector<Vertex> vertices,
                std::vector<unsigned int> indices,
                const std::string& texture_path) {

  if (is_initialized) {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
  }
  this->vertices = vertices;
  this->indices = indices;

  loadTexture(texture_path);
  setupMesh();
  is_initialized = true;
}

void Mesh::draw() {
  if (!is_initialized) {
    return;
  }
  /*
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
    if (shader != nullptr) {
      // TODO and multiple shaders????
      glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);
    }
    // and finally bind the texture
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }
  */

  glBindTexture(GL_TEXTURE_2D, texture);
  checkError("draw0");
  if (shader != nullptr) {
    shader->use();
  }
  if (shader_sf != nullptr) {
    sf::Shader::bind(shader_sf.get());
  }

  // draw mesh
  glBindVertexArray(VAO);
  checkError("draw1");
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
  checkError("draw");
  glBindVertexArray(0);
  checkError("draw2");

  if (shader != nullptr) {
    glUseProgram(0);
  }
  if (shader_sf != nullptr) {
    sf::Shader::bind(nullptr);
  }
  checkError("draw3");

  // always good practice to set everything back to defaults once configured.
  glActiveTexture(GL_TEXTURE0);
  checkError("draw4");
}

void Mesh::loadTexture(const std::string& texture_path) {
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);  // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image, create texture and generate mipmaps
  int width, height, nrChannels;

  unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    F_ERROR("Failed to load textre from %s.", texture_path.c_str());
  }
  stbi_image_free(data);
  checkError("loadTexture");
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh() {
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
  glVertexAttribPointer(POSITION_LOCATION,
                        COORDS_PER_VERTEX,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        reinterpret_cast<void*>(0));
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(NORMAL_LOCATION,
                        COORDS_PER_VERTEX,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, Normal)));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(TEXTURE_LOCATION,
                        COORDS_PER_TEXTURE,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
  // vertex tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(TANGENT_LOCATION,
                        COORDS_PER_VERTEX,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, Tangent)));
  // vertex bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(BITTANGENT_LOCATION,
                        COORDS_PER_VERTEX,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, Bitangent)));

  glBindVertexArray(0);

  checkError("setUpMesh");
}

void Mesh::checkError(const std::string& operation) {
  GLenum e;
  do {
    e = glGetError();
    if (e == GL_INVALID_ENUM) {
      F_ERROR(
          "[%s] An unacceptable value is specified for an enumerated argument. "
          "The offending command is ignored and has no other side effect than "
          "to set the error flag.",
          operation.c_str());
    } else if (e == GL_INVALID_VALUE) {
      F_ERROR(
          "[%s] A numeric argument is out of range. The offending command is "
          "ignored and has no other side effect than to set the error flag.",
          operation.c_str());
    } else if (e == GL_INVALID_OPERATION) {
      F_ERROR(
          "[%s] The specified operation is not allowed in the current state. "
          "The offending command is ignored and has no other side effect than "
          "to set the error flag",
          operation.c_str());
    } else if (e == GL_INVALID_FRAMEBUFFER_OPERATION) {
      F_ERROR(
          "[%s] The framebuffer object is not complete. The offending command "
          "is ignored and has no other side effect than to set the error flag.",
          operation.c_str());
    } else if (e == GL_OUT_OF_MEMORY) {
      F_ERROR(
          "[%s] There is not enough memory left to execute the command. The "
          "state of the GL is undefined, except for the state of the error "
          "flags, after this error is recorded.",
          operation.c_str());
    } else if (e == GL_STACK_UNDERFLOW) {
      F_ERROR(
          "[%s] An attempt has been made to perform an operation that would "
          "cause an internal stack to underflow.",
          operation.c_str());
    } else if (e == GL_STACK_OVERFLOW) {
      F_ERROR(
          "[%s] An attempt has been made to perform an operation that would "
          "cause an internal stack to overflow.",
          operation.c_str());
    } else if (e != GL_NO_ERROR) {
      F_ERROR("[%s] An unknown error [%u] occured.", operation.c_str(), e);
    }
  } while (e != GL_NO_ERROR);
}

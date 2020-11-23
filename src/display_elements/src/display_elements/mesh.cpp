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
    glDeleteBuffers(1, &EBO);
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

void Mesh::loadTexture(const std::string& texture_path) {
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

  unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
    glCheck(glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
    glCheck(glGenerateMipmap(GL_TEXTURE_2D));
  } else {
    F_ERROR("Failed to load textre from %s.", texture_path.c_str());
  }
  stbi_image_free(data);
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh() {
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
      GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));

  glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
  glCheck(glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
}

void Mesh::connectShader(unsigned int shaderProgram) {

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
          "Trying to connect to shader variable %s failed. Variable not found",
          var_name.c_str());
      return;
    }
    // Check errors in case the compiler did not optimize away
    // but rather I did somthing wrong here...
    glCheck();
    const unsigned int u_pos = static_cast<unsigned int>(variable_position);
    glCheck(glEnableVertexAttribArray(u_pos));
    glCheck(glVertexAttribPointer(
        u_pos, num_values, GL_FLOAT, GL_FALSE, sizeof(Vertex), start_position));
  };

  // set the vertex attribute pointers
  // vertex Positions
  assignShaderVariable("meshPos", COORDS_PER_VERTEX, reinterpret_cast<void*>(0));

  // vertex normals
  assignShaderVariable(
      "meshNormal", COORDS_PER_VERTEX, reinterpret_cast<void*>(offsetof(Vertex, Normal)));

  // vertex texture coords
  assignShaderVariable("meshTexture",
                       COORDS_PER_TEXTURE,
                       reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

  // vertex tangent
  assignShaderVariable("meshTangent",
                       COORDS_PER_VERTEX,
                       reinterpret_cast<void*>(offsetof(Vertex, Tangent)));

  // vertex bitangent
  assignShaderVariable("meshBittangent",
                       COORDS_PER_VERTEX,
                       reinterpret_cast<void*>(offsetof(Vertex, Bitangent)));
}

void Mesh::checkError(const char* file, unsigned int line, const char* expression) {
  GLenum e;
  do {
    e = glGetError();
    std::string error;
    if (e == GL_NO_ERROR) {
      return;
    } else if (e == GL_INVALID_ENUM) {
      error = "An unacceptable value is specified for an enumerated argument.";
    } else if (e == GL_INVALID_VALUE) {
      error = "A numeric argument is out of range.";
    } else if (e == GL_INVALID_OPERATION) {
      error = "The specified operation is not allowed in the current state. ";
    } else if (e == GL_INVALID_FRAMEBUFFER_OPERATION) {
      error = "The framebuffer object is not complete.";
    } else if (e == GL_OUT_OF_MEMORY) {
      error = "There is not enough memory left to execute the command.";
    } else if (e == GL_STACK_UNDERFLOW) {
      error =
          "An attempt has been made to perform an operation that would "
          "cause an internal stack to underflow.";
    } else if (e == GL_STACK_OVERFLOW) {
      error =
          "An attempt has been made to perform an operation that would "
          "cause an internal stack to overflow.";
    } else {
      error = "An unknown error " + std::to_string(e) + " occured.";
    }
    F_ERROR(
        "\nAn OpenGL call failed in Line: %u %s \nFunction:\n %s "
        "\nError "
        "description: %s\n",
        line,
        file,
        expression,
        error.c_str());

  } while (e != GL_NO_ERROR);
}

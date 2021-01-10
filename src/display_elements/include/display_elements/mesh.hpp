#ifndef MESH_H
#define MESH_H


#include <stb/stb_image.h>

#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <array>
#include <display_elements/displayUtils.hpp>
#include <display_elements/glad_import.hpp>
#include <display_elements/shader.hpp>
#include <display_elements/vertex.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <globals/globals.hpp>
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
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  BaseMesh() {}
  virtual ~BaseMesh() {}
  virtual void draw(bool draw_shadows) = 0;

  void setDebugNormals(bool debug) {
    if (debug) {
      if (normals == nullptr) {
        calculateNormalMesh();
      }
      normals->setTransformMesh2World(transform_mesh2world);
      normals->setProjection(projection);
      normals->setView(view);
    }
    debug_normals = debug;
  }

  void setTransformMesh2World(const Eigen::Isometry3d& p) {
    transform_mesh2world = p;
    updatePose();
  }

  void translate(const Eigen::Vector3d& t) {
    transform_mesh2world.translate(t);
    updatePose();
  }

  void rotate(const Eigen::Vector3d& rpy) {
    transform_mesh2world.rotate(eigen_utils::rpy2RotationMatrix(rpy));
    updatePose();
  }

  void rotateAround(const Eigen::Vector3d& rpy, const Eigen::Vector3d& p) {
    Eigen::Vector3d diff = transform_mesh2world.translation() - p;
    transform_mesh2world.translate(-diff);
    transform_mesh2world.rotate(eigen_utils::rpy2RotationMatrix(rpy));
    transform_mesh2world.translate(diff);
  }

  // view is camera transformation world2camera
  void setView(const Eigen::Isometry3d& view_world2camera) {
    if (shader_camera != nullptr) {
      glCheck(shader_camera->use());
      glCheck(shader_camera->setMat4(SHADER_UNIFORM_VIEW_NAME, view_world2camera.matrix()));
      glUseProgram(0);
    }
    if (debug_normals && normals) {
      normals->setView(view_world2camera);
    }
    if (shader_shadow != nullptr) {
      glCheck(shader_shadow->use());
      glCheck(shader_shadow->setMat4(SHADER_UNIFORM_VIEW_NAME, view_world2camera.matrix()));
      glUseProgram(0);
    }
    this->view = view_world2camera;
  }

  void setLight(const std::shared_ptr<Light>& light) {
    this->light = light;
    if (shader_camera != nullptr) {
      glCheck(shader_camera->use());
      glCheck(shader_camera->setVec3(SHADER_UNIFORM_LIGHT_POSITION_NAME,
                                     light->getPosition()));
      glCheck(shader_camera->setVec3(SHADER_UNIFORM_LIGHT_DIRECTION_NAME,
                                     light->getDirection()));
      glCheck(shader_camera->setVec3(SHADER_UNIFORM_LIGHT_AMBIENT_NAME, light->getAmbient()));
      glCheck(shader_camera->setVec3(SHADER_UNIFORM_LIGHT_COLOR_NAME, light->getColor()));
      glCheck(shader_camera->setMat4(SHADER_UNIFORM_LIGHT_SPACE_MATRIX_NAME,
                                     light->getLightSpaceMatrix().matrix()));

      /* debug light point of view (light->getLightSpaceMatrix())
      const Eigen::Vector3d rotate(0, M_PI, 0);
      const Eigen::Isometry3d R = eigen_utils::rpy2Isometry(rotate);

      Eigen::Isometry3d V = R * light->getPose().inverse().cast<double>();
      setView(V);

      Eigen::Projective3d P = light->getLightOrthProjection().cast<double>();
      setProjection(P);
      */

      glUseProgram(0);
    }
    if (shader_shadow != nullptr) {
      glCheck(shader_shadow->use());
      glCheck(shader_shadow->setMat4(SHADER_UNIFORM_LIGHT_SPACE_MATRIX_NAME,
                                     light->getLightSpaceMatrix().matrix()));

      glUseProgram(0);
    }
  }

  void setCameraPosition(const Eigen::Vector3d& pos) {
    if (shader_camera != nullptr) {
      glCheck(shader_camera->use());
      glCheck(shader_camera->setVec3(SHADER_UNIFORM_CAMERA_POSITION_NAME, pos));
      glUseProgram(0);
    }
  }

  void setProjection(const Eigen::Projective3d& projection) {
    if (shader_camera != nullptr) {
      glCheck(shader_camera->use());
      glCheck(shader_camera->setMat4(SHADER_UNIFORM_PROJECTION_NAME, projection.matrix()));
      glUseProgram(0);
    }
    if (debug_normals && normals) {
      normals->setProjection(projection);
    }
    this->projection = projection;
  }

  void setObjectTextures() {
    if (shader_camera != nullptr) {
      glCheck(shader_camera->use());
      glCheck(shader_camera->setInt(SHADER_UNIFORM_CAMERA_OBJECT_TEXTURE_NAME,
                                    SHADER_UNIFORM_CAMERA_OBJECT_TEXTURE_ID));
      glCheck(shader_camera->setInt(SHADER_UNIFORM_CAMERA_SHADOW_TEXTURE_NAME,
                                    SHADER_UNIFORM_CAMERA_SHADOW_TEXTURE_ID));
      glUseProgram(0);
    }
  }

  void setShadowMap() {
    if (shader_camera != nullptr) {
      glCheck(shader_shadow->use());
      glCheck(shader_shadow->setInt(SHADER_UNIFORM_SHADOW_TEXTURE_NAME,
                                    SHADER_UNIFORM_SHADOW_TEXTURE_ID));
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
    shader_camera = std::make_shared<Shader>(vertex_shader_file.c_str(),
                                             fragment_shader_file.c_str());
    if (!shader_camera->isReady()) {
      shader_camera.reset();
      return false;
    }
    connectShader(shader_camera->ID);

    // TODO

    const std::string path = Globals::getInstance().getAbsPath2Shaders();
    const std::string shadow_vs = path + "shadow_mapping.vs";
    const std::string shadow_fs = path + "shadow_mapping.fs";

    shader_shadow = std::make_shared<Shader>(shadow_vs.c_str(), shadow_fs.c_str());
    if (!shader_shadow->isReady()) {
      shader_shadow.reset();
      F_ERROR(
          "Failed to load shaddow Shader. Error in Shader? Do the files exist? "
          "{%s, "
          "%s} ",
          shadow_vs.c_str(),
          shadow_fs.c_str());
    }
    connectShadowShader(shader_shadow->ID);

    return true;
  }

 protected:
  virtual void calculateNormalMesh() = 0;

  void setMaterial(const Material& material) {
    this->material = material;
    if (shader_camera != nullptr) {
      glCheck(shader_camera->use());
      glCheck(shader_camera->setVec3(SHADER_UNIFORM_MATERIAL_SELFGLOW_NAME,
                                     material.self_glow));
      glCheck(shader_camera->setVec3(SHADER_UNIFORM_MATERIAL_DIFFUSE_NAME,
                                     material.diffuse));
      glCheck(shader_camera->setVec3(SHADER_UNIFORM_MATERIAL_SPECULAR_NAME,
                                     material.specular));
      glCheck(shader_camera->setFloat(SHADER_UNIFORM_MATERIAL_SHININESS_NAME,
                                      material.shininess));
      glUseProgram(0);
    }
  }

  /*!
   * \brief Load texture and talk to openGL to request some space for the
   * texture.
   * \param texture_path The path to the texture/image.
   */
  void loadTexture(const std::string& texture_path) {
    glCheck(glGenTextures(1, &texture));

    glCheck(glBindTexture(GL_TEXTURE_2D, texture));  // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    if (texture == 0) {
      ERROR("Cant create texture. Do we have context???");
    }
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;

    unsigned char* data =
        stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);
    if (data == nullptr) {
      F_ERROR("Failed to load textre from %s.", texture_path.c_str());
      return;
    }
    GLenum format;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;

    glCheck(glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data));
    // glCheck(glGenerateMipmap(GL_TEXTURE_2D)); // TODO this minimizes data

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

  void calculateMeshApproximation() {
    // TODO https://github.com/kmammou/v-hacd
    WARNING("TODO");
  }

  [[nodiscard]] bool activateShader(bool draw_shadow) {
    if (draw_shadow) {
      if (shader_shadow != nullptr) {
        glCheck(shader_shadow->use());
        return true;
      }
      return false;
    } else {
      if (shader_camera != nullptr) {
        glCheck(shader_camera->use());
        return true;
      }
      return false;
    }
  }

  // render data
  unsigned int VBO, EBO;

  std::shared_ptr<Shader> shader_camera = nullptr;
  std::shared_ptr<Shader> shader_shadow = nullptr;

  virtual void connectShader(unsigned int shaderProgram) = 0;
  virtual void connectShadowShader(unsigned int shaderProgram) = 0;

  // mesh Data
  unsigned int texture;
  unsigned int VAO;

  std::shared_ptr<Light> light = nullptr;

  // access in shader like this:
  // in vec3 vertexPos;
  static constexpr const char* SHADER_IN_POSITION_NAME = "vertexPos";
  static constexpr const char* SHADER_IN_NORMAL_NAME = "vertexNormal";
  static constexpr const char* SHADER_IN_TANGENT_NAME = "vertexTangent";
  static constexpr const char* SHADER_IN_BITANGENT_NAME = "vertexBitangent";
  static constexpr const char* SHADER_IN_TEXTURE_NAME = "vertexTexturePos";
  static constexpr const char* SHADER_IN_COLOR_NAME = "vertexColor";

  // shader standard uniform
  static constexpr const char* SHADER_UNIFORM_POSE_NAME = "transformMesh2World";
  static constexpr const char* SHADER_UNIFORM_VIEW_NAME =
      "transformWorld2camera";
  static constexpr const char* SHADER_UNIFORM_CAMERA_POSITION_NAME =
      "cameraPos";
  static constexpr const char* SHADER_UNIFORM_PROJECTION_NAME = "projection";
  static constexpr const char* SHADER_UNIFORM_LIGHT_SPACE_MATRIX_NAME =
      "lightSpaceMatrix";
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

  Eigen::Isometry3d transform_mesh2world = Eigen::Isometry3d::Identity();
  Eigen::Projective3d projection = Eigen::Projective3d::Identity();
  Eigen::Isometry3d view = Eigen::Isometry3d::Identity();
  Material material;
  bool debug_normals = false;
  bool is_initialized = false;
  std::shared_ptr<BaseMesh> normals = nullptr;

 private:
  void updatePose() {
    if (shader_camera != nullptr) {
      glCheck(shader_camera->use());
      glCheck(shader_camera->setMat4(SHADER_UNIFORM_POSE_NAME,
                                     transform_mesh2world.matrix()));
      glUseProgram(0);
    }
    if (debug_normals && normals) {
      normals->setTransformMesh2World(transform_mesh2world);
    }
    if (shader_shadow != nullptr) {
      glCheck(shader_shadow->use());
      glCheck(shader_shadow->setMat4(SHADER_UNIFORM_POSE_NAME,
                                     transform_mesh2world.matrix()));
      glUseProgram(0);
    }
  }
};

template <bool has_position = true, bool has_normal = true, bool has_tangent = true, bool has_bitangent = true, bool has_texture = true, bool has_color = true, int num_color_values = 3>
class Mesh : public BaseMesh {
 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
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

    loadTexture(texture_path);
    init(vertices, indices);
  }

  /*!
   * \brief Reserves space on GPU for texture and vertices
   * \param vertices The vector of vertices describeing the mesh and what not.
   * \param indices A vector of indices describeing the order of the vertices.
   * \param texture_path The path to the texture.
   */
  void init(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices) {
    if (is_initialized) {
      clean();
    }
    this->vertices = vertices;
    this->indices = indices;
    setupMesh();
    is_initialized = true;
    return;
    // TODO for some reason calculating the normals direct deletes the real mesh!?!?!?
    if (debug_normals) {
      calculateNormalMesh();
    }
  }

  /*!
   * \brief This renders the mesh using the active shader if set.
   */
  void draw(bool draw_shadows) override {

    // TODO deal with uninitialized material, light, etc

    if (!is_initialized) {
      return;
    }

    if (draw_shadows) {
      if (light == nullptr || !light->hasShadow()) {
        return;
      }
    } else {
      if (light && light->hasShadow()) {
        glCheck(glActiveTexture(GL_TEXTURE1));
        glCheck(glBindTexture(GL_TEXTURE_2D, light->getDepthMapTexture()));
      }
    }

    if constexpr (has_texture) {
      glActiveTexture(GL_TEXTURE0);
      glCheck(glBindTexture(GL_TEXTURE_2D, texture));
    }
    const bool shader_activated = activateShader(draw_shadows);

    // draw mesh
    glCheck(glBindVertexArray(VAO));
    glCheck(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr));
    glCheck(glBindVertexArray(0));

    if (shader_activated) {
      glCheck(glUseProgram(0));
    }

    // always good practice to set everything back to defaults once configured.
    if constexpr (has_texture) {
      glCheck(glActiveTexture(GL_TEXTURE0));
      glCheck(glBindTexture(GL_TEXTURE_2D, 0));
      glCheck(glActiveTexture(GL_TEXTURE1));
      glCheck(glBindTexture(GL_TEXTURE_2D, 0));
    }

    if (!draw_shadows && debug_normals && normals) {
      normals->draw(false);
    }
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

    glCheck(glBufferData(
        GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexType), &vertices[0], GL_STATIC_DRAW));

    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    glCheck(glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));

    // glCheck(glBindVertexArray(0));
    // glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  }

  /*!
   * \brief Connects the vertex shadow shader input variables with the vertex
   * array. The input variables expected in the shader are: in vec3 meshPos;
   */
  void connectShadowShader(unsigned int shaderProgram) override {
    if constexpr (has_position) {
      disp_utils::assignShaderVariable(
          shaderProgram,
          SHADER_IN_POSITION_NAME,
          VertexType::NUM_POSITION,
          sizeof(VertexType),
          reinterpret_cast<void*>(offsetof(VertexType, position)));
    }
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
    // set the vertex attribute pointers
    // vertex positions
    if constexpr (has_position) {
      disp_utils::assignShaderVariable(
          shaderProgram,
          SHADER_IN_POSITION_NAME,
          VertexType::NUM_POSITION,
          sizeof(VertexType),
          reinterpret_cast<void*>(offsetof(VertexType, position)));
    }

    // vertex normals
    if constexpr (has_normal) {
      disp_utils::assignShaderVariable(
          shaderProgram,
          SHADER_IN_NORMAL_NAME,
          VertexType::NUM_NORMAL,
          sizeof(VertexType),
          reinterpret_cast<void*>(offsetof(VertexType, normal)));
    }

    // vertex tangent
    if constexpr (has_tangent) {
      disp_utils::assignShaderVariable(
          shaderProgram,
          SHADER_IN_TANGENT_NAME,
          VertexType::NUM_TANGENT,
          sizeof(VertexType),
          reinterpret_cast<void*>(offsetof(VertexType, tangent)));
    }

    // vertex bitangent
    if constexpr (has_bitangent) {
      disp_utils::assignShaderVariable(
          shaderProgram,
          SHADER_IN_BITANGENT_NAME,
          VertexType::NUM_BITANGENT,
          sizeof(VertexType),
          reinterpret_cast<void*>(offsetof(VertexType, bitangent)));
    }

    // vertex texture coords
    if constexpr (has_texture) {
      disp_utils::assignShaderVariable(
          shaderProgram,
          SHADER_IN_TEXTURE_NAME,
          VertexType::NUM_TEXTURE,
          sizeof(VertexType),
          reinterpret_cast<void*>(offsetof(VertexType, texture_pos)));
    }

    // vertex color coords
    if constexpr (has_color) {
      disp_utils::assignShaderVariable(
          shaderProgram,
          SHADER_IN_COLOR_NAME,
          VertexType::NUM_COLOR,
          sizeof(VertexType),
          reinterpret_cast<void*>(offsetof(VertexType, color)));
    }
  }

  void calculateNormalMesh() override {

    // put very tall tetraeder on center of every vertex triangle as a normal.
    using VertexNormalType = Vertex<true, false, false, false, false, true, 3>;
    using MeshType = Mesh<true, false, false, false, false, true, 3>;

    const size_t num_triangles = indices.size() / 3;
    std::vector<VertexNormalType> verices_temp;
    std::vector<unsigned int> indices_temp;
    verices_temp.reserve(num_triangles * 4);
    indices_temp.reserve(num_triangles * 9);

    constexpr float normal_thickness = 0.01f;
    constexpr float normal_length = 0.2f;
    constexpr float r = 1.f;
    constexpr float g = 1.f;
    constexpr float b = 1.f;

    auto putInVec = [&r, &g, &b, &verices_temp](const Eigen::Vector3f& v) {
      verices_temp.emplace_back(VertexNormalType({v.x(), v.y(), v.z(), r, g, b}));
    };

    unsigned int index_nr = 0;
    const int num_indices = indices.size();
    for (int i = 0; i < num_indices; i = i + 3) {
      const Eigen::Vector3f v1(vertices[indices[i]].position);
      const Eigen::Vector3f v2(vertices[indices[i + 1]].position);
      const Eigen::Vector3f v3(vertices[indices[i + 2]].position);

      Eigen::Vector3f normal;
      if constexpr (has_normal) {
        Eigen::Vector3f normal1(vertices[indices[i]].normal);
        Eigen::Vector3f normal2(vertices[indices[i + 1]].normal);
        Eigen::Vector3f normal3(vertices[indices[i + 2]].normal);
        normal = (normal1 + normal2 + normal3) / 3.f;
      } else {
        // assuming math positive defined vertex triangle
        normal = eigen_utils::getTrianglesNormal(v1, v2, v3);
      }
      const Eigen::Vector3f centroid = (v1 + v2 + v3) / 3.f;

      const Eigen::Vector3f nv0 = centroid + (normal * normal_length);
      Eigen::Vector3f dir = v1 - centroid;
      dir.normalize();
      const Eigen::Vector3f nv1 = centroid + dir * normal_thickness;
      dir = v2 - centroid;
      dir.normalize();
      const Eigen::Vector3f nv2 = centroid + dir * normal_thickness;
      dir = v3 - centroid;
      dir.normalize();
      const Eigen::Vector3f nv3 = centroid + dir * normal_thickness;

      putInVec(nv0);
      putInVec(nv1);
      putInVec(nv2);
      putInVec(nv3);

      indices_temp.push_back(index_nr + 3);
      indices_temp.push_back(index_nr + 1);
      indices_temp.push_back(index_nr);

      indices_temp.push_back(index_nr + 2);
      indices_temp.push_back(index_nr + 3);
      indices_temp.push_back(index_nr);

      indices_temp.push_back(index_nr + 1);
      indices_temp.push_back(index_nr + 2);
      indices_temp.push_back(index_nr);

      index_nr += 4;
    }

    // initiate normal Mesh
    std::shared_ptr<MeshType> normalsMesh = std::make_shared<MeshType>();

    normalsMesh->init(verices_temp, indices_temp);

    const std::string path = Globals::getInstance().getAbsPath2Shaders();
    const std::string vs = path + "normal.vs";
    const std::string fs = path + "normal.fs";
    if (!normalsMesh->loadShader(vs, fs)) {
      F_ERROR(
          "Failed to load Shader. Error in Shader? Do the files exist? {%s, "
          "%s} ",
          vs.c_str(),
          fs.c_str());
    }
    addNormals(normalsMesh);
    // this->normals = std::dynamic_pointer_cast<BaseMesh> (normals);
  }

  void addNormals(const std::shared_ptr<BaseMesh>& n) { normals = n; }
};

#endif

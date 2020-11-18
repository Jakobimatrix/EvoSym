#ifndef WORLD
#define WORLD

#include <display/displayUtils.hpp>
#include <display/mesh.hpp>
#include <display/shader.hpp>
#include <memory>
#include <string>

class World {
 public:
  World();

  void create_mesh();

  [[nodiscard]] bool load_mesh(const std::string& file);

  void update();

  // disp_utils::MeshShaderPair getWorldsMeshShaderPair() const {
  //  return disp_utils::makeMeshShaderPair(world_mesh, world_shader);
  //}

  [[nodiscard]] bool save(const std::string& file);
  [[nodiscard]] bool load(const std::string& file);


 private:
  // std::shared_ptr<Mesh> world_mesh = nullptr;
  // std::shared_ptr<Shader> world_shader = nullptr;
};
#endif

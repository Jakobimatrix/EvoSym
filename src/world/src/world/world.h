#ifndef WORLD
#define WORLD

#include <display/mesh.hpp>
#include <memory>
#include <string>

class World {
 public:
  World();

  void update();

  std::shared_ptr<Mesh> getWorldMesh() {
    return std::make_shared<Mesh>(world_mesh);
  }

  [[nodiscard]] bool save(const std::string& file);
  [[nodiscard]] bool load(const std::string& file);


 private:
  Mesh world_mesh;
};
#endif

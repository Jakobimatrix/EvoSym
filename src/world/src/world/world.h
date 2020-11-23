#ifndef WORLD
#define WORLD

#include <display_elements/worldMesh.h>

#include <memory>
#include <string>

class World {
 public:
  World();

  void init();

  void create_mesh();

  [[nodiscard]] bool load_mesh(const std::string& file);

  void update();

  std::shared_ptr<Mesh> getWorldsMesh() const { return world_mesh; }

  [[nodiscard]] bool save(const std::string& file);
  [[nodiscard]] bool load(const std::string& file);


 private:
  std::shared_ptr<Mesh> world_mesh = nullptr;
};
#endif

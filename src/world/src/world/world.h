#ifndef WORLD
#define WORLD

#include <display/mesh.h>

#include <memory>

class World {
 public:
  World() {}
  void update();

  std::shared_ptr<Mesh> getWorldMesh() {
    return std::make_shared<Mesh>(world_mesh);
  }

  void save();


 private:
  Mesh world_mesh;
};
#endif

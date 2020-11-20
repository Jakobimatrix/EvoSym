#ifndef SFML_RENDER_WINDOW
#define SFML_RENDER_WINDOW

#include <SFML/Graphics.hpp>
#include <cmath>
#include <display/displayUtils.hpp>

class SfmlRenderWindow : public sf::RenderWindow{
public:
  SfmlRenderWindow();

  void init();

  void update();

  [[nodiscard]] unsigned long addMesh(const disp_utils::MeshShaderPair& mesh_shader_pair);

  [[nodiscard]] bool removeMesh(unsigned long id);

 private:
  void processInputActions();

  void setPerspective();

  void drawMesh();
  void draw2DStack();

  // SFML entities
  sf::Vector2i last_mouse_pos = sf::Vector2i(0, 0);

  std::map<unsigned long, const disp_utils::MeshShaderPair> meshes;
  unsigned long mesh_counter = 0;
  bool is_initialized = false;

};

#endif

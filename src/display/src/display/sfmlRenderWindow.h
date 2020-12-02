#ifndef SFML_RENDER_WINDOW
#define SFML_RENDER_WINDOW


#include <display_elements/worldMesh.h>

#include <SFML/Graphics.hpp>
#include <chrono>
#include <cmath>
#include <display_elements/camera.hpp>
#include <display_elements/displayUtils.hpp>
#include <display_elements/mesh.hpp>
#include <timer/timer.hpp>

class SfmlRenderWindow : public sf::RenderWindow {
 public:
  SfmlRenderWindow();

  void init(sf::WindowHandle handle);

  void update();

  [[nodiscard]] unsigned long addMesh(const std::shared_ptr<BaseMesh> &simple_mesh);

  [[nodiscard]] bool removeMesh(unsigned long id);

  void onResize() override;

  bool isInitialized() { return is_initialized; }

  // sfml can not detect mouse wheel scroll, this baaad hack
  void scrollHack(double f);

 private:
  void processInputActions();
  void processMouseAction();
  void dragMouseLeft(const sf::Vector2i &diff);
  void dragMouseRight(const sf::Vector2i &diff);
  void leftKlick(const sf::Vector2i &mouse_pos);
  void rightKlick(const sf::Vector2i &mouse_pos);

  void updateMeshesView();
  void updateMeshesPerspective();


  void setPerspective();

  void drawMesh();
  void draw2DStack();

  // SFML entities
  Camera camera = Camera();
  sf::Vector2i last_mouse_pos = sf::Vector2i(0, 0);
  const sf::Vector2i STRANGE_MOUSE_OFFSET = sf::Vector2i(0, 60);
  sf::Clock clock;

  double window_ratio = 1;


  std::map<unsigned long, const std::shared_ptr<BaseMesh>> meshes;
  unsigned long mesh_counter = 0;

  std::shared_ptr<WorldMesh> world_mesh = nullptr;
  bool is_initialized = false;
  bool has_focus = true;

  tool::SingleTimer mouse_left_timer;
  tool::SingleTimer mouse_right_timer;

  static constexpr std::chrono::milliseconds MAX_KLICK_DURATION =
      std::chrono::milliseconds(300);
};

#endif

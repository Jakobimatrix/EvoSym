#ifndef SFML_RENDER_WINDOW
#define SFML_RENDER_WINDOW

#include <SFML/Graphics.hpp>
#include <cmath>
#include <display/displayUtils.hpp>
#include <display/worldMesh.hpp>
#include <chrono>
#include <timer/timer.hpp>

class SfmlRenderWindow : public sf::RenderWindow{
public:
  SfmlRenderWindow();

  void init();

  void update();

  [[nodiscard]] unsigned long addMesh(const disp_utils::MeshShaderPair& mesh_shader_pair);

  [[nodiscard]] bool removeMesh(unsigned long id);

  [[nodiscard]] unsigned long addSimpleMesh(const std::shared_ptr<SimpleMesh>& simple_mesh);

  [[nodiscard]] bool removeSimpleMesh(unsigned long id);

  void onResize() override;

  bool isInitialized(){return is_initialized;}

 private:
  void processInputActions();
  void processMouseAction();
  void dragMouseLeft(const sf::Vector2i &diff);
  void dragMouseRight(const sf::Vector2i &diff);
  void leftKlick(const sf::Vector2i &mouse_pos);
  void rightKlick(const sf::Vector2i &mouse_pos);


  void setPerspective();

  void drawMesh();
  void draw2DStack();

  // SFML entities
  sf::Vector2i last_mouse_pos = sf::Vector2i(0, 0);
  const sf::Vector2i STRANGE_MOUSE_OFFSET = sf::Vector2i(0,60);
  sf::Clock clock;


  std::map<unsigned long, const disp_utils::MeshShaderPair> meshes;
  unsigned long mesh_counter = 0;
  std::map<unsigned long, const std::shared_ptr<SimpleMesh>> simple_meshes;
  unsigned long simple_mesh_counter = 0;

  WorldMesh wm;
  bool is_initialized = false;
  bool has_focus = true;

  tool::SingleTimer mouse_left_timer;
  tool::SingleTimer mouse_right_timer;

  static constexpr std::chrono::milliseconds MAX_KLICK_DURATION = std::chrono::milliseconds(300);
};

#endif

#ifndef SFML_RENDER_WINDOW
#define SFML_RENDER_WINDOW


#include <display_elements/sun.h>
#include <display_elements/worldMesh.h>

#include <SFML/Graphics.hpp>
#include <chrono>
#include <cmath>
#include <display_elements/camera.hpp>
#include <display_elements/displayUtils.hpp>
#include <display_elements/light.hpp>
#include <display_elements/mesh.hpp>
#include <timer/timer.hpp>

class SfmlRenderWindow : public sf::RenderWindow {
 public:
  SfmlRenderWindow();

  void init(const sf::WindowHandle &handle);

  void update();

  [[nodiscard]] unsigned long addMesh(const std::shared_ptr<BaseMesh> &simple_mesh);

  [[nodiscard]] bool removeMesh(unsigned long id);

  void onResize() override;

  bool isInitialized() { return is_initialized; }

  // sfml can not detect mouse wheel scroll, this baaad hack
  void scrollHack(double f);

  /*!
   * \brief En- or disables the OpenGL rendering for the window.
   * In debug mode an error is thrown if the operation was not possible.
   * \param activate Bool to decide wheather to activate or deactivate.
   */
  void setActive(bool activate) {
    if (is_active == activate) {
      return;
    }
    const bool success = sf::RenderWindow::setActive(activate);
    is_active = activate;
    if (!success) {
      const std::string state = is_active ? "deactivate" : "activate";
      F_ERROR("Was not able to set the window for OpenGL rendering %s", state.c_str());
    }
  }

 private:
  void initOpenGl(const sf::WindowHandle &handle);

  void initCamera();

  void updateWindowRatio();

  /*!
   * \brief Activates the OpenGL rendering for the window if it was deactivated.
   * You have to deactivate the rendering using deactivateIf() in order to not
   * break the state machine.
   */
  void activateIf() {
    activation_count++;
    setActive(true);
  }

  /*!
   * \brief Deaktivates the OpenGL rendering for the window if it was deactivated before calling activateIf().
   * This function is supposed to be called after activateIf() was used.
   */
  void deactivateIf() {
    activation_count--;

    assert(activation_count >= 0 &&
           "SfmlRenderWindow::deactivateIf is supposed to be used after "
           "activateIf().");
    if (activation_count <= 0) {
      setActive(false);
    }
  }

  void processInputActions();
  void processMouseAction();
  void processKeyPressedAction();
  void dragMouseLeft(const sf::Vector2i &diff);
  void dragMouseRight(const sf::Vector2i &diff);
  void leftKlick(const sf::Vector2i &mouse_pos);
  void rightKlick(const sf::Vector2i &mouse_pos);

  void onCameraPositionUpdate();
  void onCameraPerspectiveUpdate();
  void onLightChange();

  void enable3dDepth();
  void setPerspective();

  void drawMesh();
  void drawShadows();
  void draw2DStack();

  void printGraphicCardInformation();

  // SFML entities
  Camera camera = Camera();
  sf::Vector2i last_mouse_pos = sf::Vector2i(0, 0);
  const sf::Vector2i STRANGE_MOUSE_OFFSET = sf::Vector2i(0, 60);

  Light light;

  double window_ratio = 1;
  bool is_active = false;

  // Not thread save!
  // state used by activateIf and deactivateIf
  int activation_count = 0;

  std::map<unsigned long, const std::shared_ptr<BaseMesh>> meshes;
  unsigned long mesh_counter = 0;

  std::shared_ptr<WorldMesh> world_mesh = nullptr;
  std::shared_ptr<WorldMesh> world_mesh2 = nullptr;

  std::shared_ptr<SunMesh> sun = nullptr;
  bool is_initialized = false;
  bool has_focus = true;

  tool::SingleTimer mouse_left_timer;
  tool::SingleTimer mouse_right_timer;

  static constexpr std::chrono::milliseconds MAX_KLICK_DURATION =
      std::chrono::milliseconds(300);
};

#endif

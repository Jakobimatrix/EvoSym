#ifndef RENDER_WINDOW
#define RENDER_WINDOW


#include <display_elements/sun.h>
#include <display_elements/worldMesh.h>

#include <chrono>
#include <cmath>
#include <display_elements/camera.hpp>
#include <display_elements/displayUtils.hpp>
#include <display_elements/light.hpp>
#include <display_elements/mesh.hpp>
#include <functional>
#include <timer/timer.hpp>


struct IsPressed {
  bool ctrl = false;
  bool alt = false;
  bool alt_gr = false;
  bool space = false;
  bool shift = false;
  bool esc = false;
  bool mouse_left = false;
  bool mouse_right = false;
  bool mouse_mid = false;
};

class RenderWindow : protected QOpenGLExtraFunctions {
 public:
  RenderWindow();

  void init();

  void update();

  [[nodiscard]] unsigned long addMesh(const std::shared_ptr<BaseMesh> &simple_mesh);

  [[nodiscard]] bool removeMesh(unsigned long id);

  void onResize(int width, int height);

  bool isInitialized() { return is_initialized; }

  void clean();

  typedef std::function<GLuint()> CallbackGetDefaultFrameBuffer;

  void setDefaultFrameBufferGetter(const CallbackGetDefaultFrameBuffer &func) {
    getDefualtFrameFuffer = func;
  }

 protected:
  void dragMouseLeft(const Eigen::Vector2i &diff);
  void dragMouseRight(const Eigen::Vector2i &diff);
  void leftKlick(const Eigen::Vector2i &mouse_pos);
  void rightKlick(const Eigen::Vector2i &mouse_pos);
  void scroll(double f);
  void keyN();

  IsPressed is_pressed;

 private:
  void initOpenGl();

  void initCamera();

  void onCameraPositionUpdate();
  void onCameraPerspectiveUpdate();
  void onLightChange();

  void enable3dDepth();
  void setPerspective();

  void drawMesh();
  void drawShadows();

  void printGraphicCardInformation();

  void animate();

  // SFML entities
  Camera camera;
  Eigen::Vector2i last_mouse_pos = Eigen::Vector2i(0, 0);

  // TODO der kommt von QT und sollte in QT verrechnet werden!!!
  const Eigen::Vector2i STRANGE_MOUSE_OFFSET = Eigen::Vector2i(0, 60);

  std::shared_ptr<Light> light_ptr;

  double window_ratio = 1;
  Eigen::Vector2i window_size;

  std::map<unsigned long, const std::shared_ptr<BaseMesh>> meshes;
  unsigned long mesh_counter = 0;

  std::shared_ptr<WorldMesh> world_mesh = nullptr;
  std::shared_ptr<SunMesh> sun_mesh = nullptr;

  bool is_initialized = false;

  CallbackGetDefaultFrameBuffer getDefualtFrameFuffer = []() { return 0; };
};

#endif

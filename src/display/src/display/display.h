#ifndef DISPLAY
#define DISPLAY
#include <SFML/Graphics.hpp>
#include <globals/globals.hpp>
#include <globals/macros.hpp>
#include <map>
#include <memory>
#include <settings.hpp>

#include "menu.h"
#include "mesh.h"

class Display : public util::Settings {
 public:
  Display(Menu* menu = nullptr)
      : Settings(Globals::getInstance().getPath2DisplaySettings()), menu(menu) {
    put<unsigned int>(disp_height, DISP_HEIGHT_ID);
    put<unsigned int>(disp_width, DISP_WIDTH_ID);
    put<unsigned int>(color_depth, COLOR_DEPTH_ID);


    if (!initWindow()) {
      ERROR("Display::Display: Was not able to initiate Display.");
    }
  }

  ~Display() { save(); }

  void draw();

  [[nodiscard]] bool isOpen() { return window.isOpen(); }

  [[nodiscard]] unsigned long addMesh(std::shared_ptr<Mesh> mesh);

  [[nodiscard]] bool removeMesh(unsigned long id);

 private:
  void processInputActions();

  [[nodiscard]] bool initWindow();

  void setPerspective();

  void drawMesh();
  void drawMenu();
  void draw2DStack();

  // SFML entities
  sf::RenderWindow window;
  sf::Vector2i last_mouse_pos = sf::Vector2i(0, 0);
  std::map<unsigned long, std::shared_ptr<Mesh>> meshes;
  unsigned long mesh_counter = 0;

  // Display settings
  unsigned int disp_height = 600;
  const std::string DISP_HEIGHT_ID = "DisplayHeightPixel";
  unsigned int disp_width = 800;
  const std::string DISP_WIDTH_ID = "DisplayWidthPixel";
  unsigned int color_depth = 24;
  const std::string COLOR_DEPTH_ID = "ColorDepth";

  Menu* menu;
};

#endif

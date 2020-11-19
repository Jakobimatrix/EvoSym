#ifndef DISPLAY
#define DISPLAY
#include <warning_guards/warning_guards.h>

#include <SFML/Graphics.hpp>
#include <display/QSFMLCanvas.hpp>
#include <display/displayUtils.hpp>
#include <display/mesh.hpp>
#include <display/shader.hpp>
#include <globals/globals.hpp>
#include <globals/macros.hpp>
#include <map>
#include <memory>

THIRD_PARTY_HEADERS_BEGIN
#include <settings.hpp>
THIRD_PARTY_HEADERS_END

class Display : public QSFMLCanvas, public util::Settings {
  Q_OBJECT
 private:
  void onInit() override;
  void onUpdate() override;

 public:
  explicit Display(QWidget* parent)
      : QSFMLCanvas(parent),
        Settings(Globals::getInstance().getPath2DisplaySettings()) {
    put<unsigned int>(disp_height, DISP_HEIGHT_ID);
    put<unsigned int>(disp_width, DISP_WIDTH_ID);
    put<unsigned int>(color_depth, COLOR_DEPTH_ID);
  }

  ~Display() override { save(); }


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

  // Display settings
  unsigned int disp_height = 600;
  const std::string DISP_HEIGHT_ID = "DisplayHeightPixel";
  unsigned int disp_width = 800;
  const std::string DISP_WIDTH_ID = "DisplayWidthPixel";
  unsigned int color_depth = 24;
  const std::string COLOR_DEPTH_ID = "ColorDepth";

 signals:

 public slots:
};

#endif

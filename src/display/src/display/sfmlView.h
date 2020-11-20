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

class SFMLView : public QSFMLCanvas {
  Q_OBJECT
 private:
  void onInit() override;
  void onUpdate() override;

 public:
  explicit SFMLView(QWidget* parent) : QSFMLCanvas(parent) {}

  ~SFMLView() override {}


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

 signals:

 public slots:
};

#endif

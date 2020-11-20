#include "sfmlRenderWindow.h"

#include "display/worldMesh.hpp"

// THIRD_PARTY_HEADERS_BEGIN
#include <glad/glad.h>
// THIRD_PARTY_HEADERS_END

SfmlRenderWindow::SfmlRenderWindow() {}

void SfmlRenderWindow::init() {
  // create(sf::VideoMode(disp_width, disp_height, color_depth), Globals::getInstance().getMainVidowTitle(), sf::Style::Default);
  setVerticalSyncEnabled(true);


  // Make the window the active window for OpenGL calls
  setActive(true);

  setPerspective();


  wm = WorldMesh();
  const std::shared_ptr<SimpleMesh> sm = std::make_shared<SimpleMesh>(wm);
  uint i = addSimpleMesh(sm);

  is_initialized = true;
}

void SfmlRenderWindow::update() {

  if (!sf::RenderWindow::isOpen() || !is_initialized) {
    return;
  }
  processInputActions();


  sf::RenderWindow::clear(sf::Color(100, 100, 100, 255));


  // second draw triangles/Meshes
  drawMesh();

  // third reset transformation
  // Save the current OpenGL render states and matrices.
  pushGLStates();
  draw2DStack();
  // Restore the previously saved OpenGL render states and matrices.
  popGLStates();
  sf::RenderWindow::display();
}

unsigned long SfmlRenderWindow::addMesh(const disp_utils::MeshShaderPair& mesh_shader_pair) {
  meshes.emplace(std::make_pair(mesh_counter, mesh_shader_pair));
  return mesh_counter++;
}

bool SfmlRenderWindow::removeMesh(unsigned long id) {
  auto ptr = meshes.find(id);
  if (ptr != meshes.end()) {
    meshes.erase(ptr);
    return true;
  }
  return false;
}

unsigned long SfmlRenderWindow::addSimpleMesh(const std::shared_ptr<SimpleMesh>& simple_mesh) {
  simple_meshes.emplace(std::make_pair(simple_mesh_counter, simple_mesh));
  return simple_mesh_counter++;
}

bool SfmlRenderWindow::removeSimpleMesh(unsigned long id) {
  auto ptr = simple_meshes.find(id);
  if (ptr != simple_meshes.end()) {
    simple_meshes.erase(ptr);
    return true;
  }
  return false;
}

void SfmlRenderWindow::draw2DStack() {
  sf::RectangleShape rectangle;
  rectangle.setSize(sf::Vector2f(10, 10));
  rectangle.setFillColor(sf::Color::Red);
  rectangle.setPosition(0, 0);
  draw(rectangle);

  const sf::Vector2i mouse_pos =
      sf::Mouse::getPosition() - sf::RenderWindow::getPosition() + STRANGE_MOUSE_OFFSET;


  sf::RectangleShape mouseUnder;
  mouseUnder.setSize(sf::Vector2f(10, 10));
  mouseUnder.setFillColor(sf::Color::Red);
  mouseUnder.setPosition(mouse_pos.x, mouse_pos.y);
  draw(mouseUnder);
}

void SfmlRenderWindow::drawMesh() {
  sf::RenderWindow::setActive(true);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, 0, -5.f);
  glRotatef(clock.getElapsedTime().asSeconds() * 15.f, 1.f, 0.f, 0.f);
  glRotatef(clock.getElapsedTime().asSeconds() * 10.f, 0.f, 1.f, 0.f);
  glRotatef(clock.getElapsedTime().asSeconds() * 30.f, 0.f, 0.f, 1.f);
  for (const auto& mesh_shader_pair : meshes) {
    // Mesh* m = mesh_shader_pair.second.first.get();
    // Shader* s = mesh_shader_pair.second.second.get();
    // m->Draw(*s);
    mesh_shader_pair.second.first.get()->Draw(*mesh_shader_pair.second.second.get());
  }
  for (const auto& simple_mesh : simple_meshes) {
    // Mesh* m = mesh_shader_pair.second.first.get();
    // Shader* s = mesh_shader_pair.second.second.get();
    // m->Draw(*s);
    simple_mesh.second.get()->draw();
  }
  glLoadIdentity();
  sf::RenderWindow::setActive(false);
}

void SfmlRenderWindow::processInputActions() {
  sf::Event event;
  while (pollEvent(event)) {

    // Close window: exit
    if (event.type == sf::Event::Closed) {
      sf::RenderWindow::close();
    } else if (event.type == sf::Event::GainedFocus) {
      this->has_focus = true;
    } else if (event.type == sf::Event::LostFocus) {
      this->has_focus = false;
    }
  }
  if (has_focus) {
    processMouseAction();
  }
}

void SfmlRenderWindow::onResize() {
  setPerspective();

  sf::View view;
  view.setSize(sf::RenderWindow::getSize().x, sf::RenderWindow::getSize().y);
  view.setCenter(sf::RenderWindow::getSize().x / 2.f,
                 sf::RenderWindow::getSize().y / 2.f);
  sf::RenderWindow::setView(view);
}

void SfmlRenderWindow::processMouseAction() {

  const sf::Vector2i mouse_pos =
      sf::Mouse::getPosition() - sf::RenderWindow::getPosition() + STRANGE_MOUSE_OFFSET;

  if (!mouse_left_timer.hasStarted() && !mouse_right_timer.hasStarted()) {
    const sf::Vector2u win_size = sf::RenderWindow::getSize();
    if (mouse_pos.x < 0 || mouse_pos.y < 0 || mouse_pos.x > win_size.x ||
        mouse_pos.y > win_size.y) {
      // not dragging and outside the window.
      return;
    }
  }

  if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
    if (!mouse_right_timer.hasStarted()) {
      mouse_right_timer.start();
    } else {
      dragMouseRight(mouse_pos - last_mouse_pos);
    }
  } else {
    if (mouse_right_timer.hasStarted()) {
      if (MAX_KLICK_DURATION > mouse_right_timer.getPassedTime<std::chrono::milliseconds>()) {
        rightKlick(mouse_pos);
      }
      mouse_right_timer.stop();
    }
  }

  if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    if (!mouse_left_timer.hasStarted()) {
      mouse_left_timer.start();
    } else {
      dragMouseLeft(mouse_pos - last_mouse_pos);
    }
  } else {
    if (mouse_left_timer.hasStarted()) {
      if (MAX_KLICK_DURATION > mouse_left_timer.getPassedTime<std::chrono::milliseconds>()) {
        leftKlick(mouse_pos);
      }
      mouse_left_timer.stop();
    }
  }


  last_mouse_pos = mouse_pos;
}

void SfmlRenderWindow::dragMouseLeft(const sf::Vector2i& diff) {
  F_DEBUG("draggin left x: %d y: %d", diff.x, diff.y);
}
void SfmlRenderWindow::dragMouseRight(const sf::Vector2i& diff) {
  F_DEBUG("draggin right x: %d y: %d", diff.x, diff.y);
}
void SfmlRenderWindow::leftKlick(const sf::Vector2i& mouse_pos) {
  F_DEBUG("klick left x: %d y: %d", mouse_pos.x, mouse_pos.y);
}
void SfmlRenderWindow::rightKlick(const sf::Vector2i& mouse_pos) {
  F_DEBUG("klick right x: %d y: %d", mouse_pos.x, mouse_pos.y);
}

void SfmlRenderWindow::setPerspective() {
  sf::RenderWindow::setActive(true);

  // Load OpenGL or OpenGL ES entry points using glad
#ifdef SFML_OPENGL_ES
  // gladLoadGLES1(reinterpret_cast<GLADloadfunc>(sf::Context::getFunction));
  ERROR("SFML_OPENGL_ES not supported")
#else
  // gladLoadGL(reinterpret_cast<GLADloadproc>(sf::Context::getFunction));
  gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction));
  // gladLoadGL();
#endif

  // Enable Z-buffer read and write
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
#ifdef SFML_OPENGL_ES
  glClearDepthf(1.f);
#else
  glClearDepth(1.);
#endif

  ///
  // TODO NEEDED EVERYTIME? PUT INTO DIFERENT FUNCTION?
  // Disable lighting
  glDisable(GL_LIGHTING);
  // Configure the viewport (the same size as the window)
  glViewport(0, 0, sf::RenderWindow::getSize().x, sf::RenderWindow::getSize().y);
  ///


  // Setup a perspective projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
#ifdef SFML_OPENGL_ES
  GLfloat ratio = static_cast<float>(sf::RenderWindow::getSize().x) /
                  sf::RenderWindow::getSize().y;
  glFrustumf(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);
#else
  GLdouble ratio = static_cast<double>(sf::RenderWindow::getSize().x) /
                   sf::RenderWindow::getSize().y;
  glFrustum(-ratio, ratio, -1., 1., 1., 500.);
#endif

  sf::RenderWindow::setActive(false);
}

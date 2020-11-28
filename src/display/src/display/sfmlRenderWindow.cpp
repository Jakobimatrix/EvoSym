#include "sfmlRenderWindow.h"

#include <SFML/OpenGL.hpp>

// THIRD_PARTY_HEADERS_BEGIN
#include <glad/glad.h>
// THIRD_PARTY_HEADERS_END

SfmlRenderWindow::SfmlRenderWindow() {}

void SfmlRenderWindow::init(sf::WindowHandle handle) {

  // Create the SFML window with the widget handle
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 4;
  settings.majorVersion = 3;
  settings.minorVersion = 0;
  sf::RenderWindow::create(handle, settings);

  camera.setPosition(0, 0, 0);

  setVerticalSyncEnabled(true);

  // Load OpenGL or OpenGL ES entry points using glad
  gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction));

  setActive(true);
  // SFML bug, first time calling isAvaiable it will deactivate the window
  // https://en.sfml-dev.org/forums/index.php?topic=17243.0
  sf::Shader::isAvailable();
  setActive(true);
  sf::Texture::getMaximumSize();
  setActive(true);


  printf("Vendor graphic card: %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version GL: %s\n", glGetString(GL_VERSION));
  printf("Version GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  world_mesh = std::make_shared<WorldMesh>();
  // set perspective for world mesh
  const Eigen::Affine3d pose = Eigen::Affine3d::Identity();
  world_mesh->setPose(pose);
  // like the "lense" we are looking through
  const float ratio = static_cast<float>(sf::RenderWindow::getSize().x) /
                      sf::RenderWindow::getSize().y;
  glm::mat4 projection = glm::perspective(
      glm::radians(30.f),  // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
      ratio,  // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
      0.1f,  // Near clipping plane. Keep as big as possible, or you'll get precision issues.
      100.0f  // Far clipping plane. Keep as little as possible.
  );

  Eigen::Affine3d projection_eigen = utils::glmMat2EigenAffine(projection);
  projection_eigen = Eigen::Affine3d::Identity();
  world_mesh->setProjection(projection_eigen);

  setActive(false);
  is_initialized = true;
}

void SfmlRenderWindow::update() {

  if (!sf::RenderWindow::isOpen() || !is_initialized) {
    return;
  }
  processInputActions();

  sf::RenderWindow::clear(sf::Color(100, 100, 100, 255));
  drawMesh();

  // not working :(
  //  // reset transformation for SFML
  //  // se:e https://www.sfml-dev.org/tutorials/2.5/window-opengl.php
  //  // Save the current OpenGL render states and matrices.
  //  // glPushAttrib(GL_ALL_ATTRIB_BITS);
  //  // glPushMatrix();
  pushGLStates();
  //  // resetGLStates();
  draw2DStack();
  //  // Restore the previously saved OpenGL render states and matrices.
  //  // glPopAttrib();
  //  // glPopMatrix();
  popGLStates();


  sf::RenderWindow::display();
}

unsigned long SfmlRenderWindow::addMesh(const std::shared_ptr<Mesh>& simple_mesh) {
  meshes.emplace(std::make_pair(mesh_counter, simple_mesh));
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

void SfmlRenderWindow::draw2DStack() {
  sf::RectangleShape rectangle;
  rectangle.setSize(sf::Vector2f(10, 10));
  rectangle.setFillColor(sf::Color::Red);
  rectangle.setPosition(0, 0);
  sf::RenderWindow::draw(rectangle);

  const sf::Vector2i mouse_pos =
      sf::Mouse::getPosition() - sf::RenderWindow::getPosition() + STRANGE_MOUSE_OFFSET;


  sf::RectangleShape mouseUnder;
  mouseUnder.setSize(sf::Vector2f(10, 10));
  mouseUnder.setFillColor(sf::Color::Red);
  mouseUnder.setPosition(mouse_pos.x, mouse_pos.y);
  sf::RenderWindow::draw(mouseUnder);
}

void SfmlRenderWindow::drawMesh() {
  sf::RenderWindow::setActive(true);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // draw world
  world_mesh->setView(camera.GetViewMatrix());

  world_mesh->draw();

  for (const auto& mesh_shader_pair : meshes) {
    mesh_shader_pair.second->draw();
  }

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
    } else if (event.type == sf::Event::MouseWheelScrolled) {
      std::cout << "wheel d: " << event.mouseWheel.delta << std::endl;
      std::cout << "wheel x: " << event.mouseWheel.x << std::endl;
      std::cout << "wheel y: " << event.mouseWheel.y << std::endl;
    } else if (event.type == sf::Event::MouseWheelMoved) {
      std::cout << "wheelscrl d: " << event.mouseWheelScroll.delta << std::endl;
      std::cout << "wheelscrl x: " << event.mouseWheelScroll.x << std::endl;
      std::cout << "wheelscrl y: " << event.mouseWheelScroll.y << std::endl;
      std::cout << "wheelscrl w: " << event.mouseWheelScroll.wheel << std::endl;
    }
  }
  if (has_focus) {
    processMouseAction();
  }
}

void SfmlRenderWindow::onResize() {
  setPerspective();
  /*
  sf::View view;
  view.setSize(sf::RenderWindow::getSize().x, sf::RenderWindow::getSize().y);
  view.setCenter(sf::RenderWindow::getSize().x / 2.f,
                 sf::RenderWindow::getSize().y / 2.f);
  sf::RenderWindow::setView(view);
  */
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

void SfmlRenderWindow::scrollHack(float f) { camera.ProcessMouseScroll(f); }
void SfmlRenderWindow::dragMouseLeft(const sf::Vector2i& diff) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
    camera.ProcessShift(diff.x, diff.y);
  } else {
    camera.ProcessRotation(diff.x, diff.y);
  }
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

  const GLdouble ratio = static_cast<double>(sf::RenderWindow::getSize().x) /
                         sf::RenderWindow::getSize().y;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Configure the viewport (the same size as the window)
  glViewport(0, 0, sf::RenderWindow::getSize().x, sf::RenderWindow::getSize().y);

  const double near = 0.1;
  const double far = 100;
  const double bottom = -1.0;
  const double top = 1.0;
  const double left = -ratio;
  const double right = ratio;
  glFrustum(left, right, bottom, top, near, far);


  sf::RenderWindow::setActive(false);
}

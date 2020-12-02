#include "sfmlRenderWindow.h"

#include <SFML/OpenGL.hpp>
#include <display_elements/glad_import.hpp>
#include <utils/math.hpp>

SfmlRenderWindow::SfmlRenderWindow() {}

void SfmlRenderWindow::init(sf::WindowHandle handle) {


  // Create the SFML window with the widget handle
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 4;
  settings.majorVersion = 3;  // 4.2 would be possible but results in error BindTexture
  settings.minorVersion = 0;
  settings.attributeFlags = sf::ContextSettings::Default;
  // settings.attributeFlags = sf::ContextSettings::Core; // changes v3 to v4
  sf::RenderWindow::create(handle, settings);
  setVerticalSyncEnabled(true);
  onResize();

  Camera::CallbackCameraChange viewChange =
      std::bind(&SfmlRenderWindow::updateMeshesView, this);
  Camera::CallbackCameraChange perspectiveChange =
      std::bind(&SfmlRenderWindow::updateMeshesPerspective, this);

  camera.addCallbackPerspectiveChange(perspectiveChange);
  camera.addCallbackViewChange(viewChange);


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


  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0, 1.0);


  // init mesh
  world_mesh = std::make_shared<WorldMesh>();
  // set perspective for world mesh
  const Eigen::Affine3f pose = Eigen::Affine3f::Identity();
  world_mesh->setPose(pose);

  unsigned int wmid = addMesh(world_mesh);

  setActive(false);

  // init meshes
  updateMeshesView();
  updateMeshesPerspective();
  is_initialized = true;
}

void SfmlRenderWindow::update() {

  if (!sf::RenderWindow::isOpen() || !is_initialized) {
    return;
  }
  processInputActions();

  // drawing sfml renderer (2Dstack resets this)
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0, 1.0);

  // sf::RenderWindow::clear(sf::Color(100, 100, 100, 255));
  glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  // glEnable(GL_COLOR_MATERIAL);
  // glEnable(GL_LIGHTING);
  // glEnable(GL_LIGHT0);
  // GLfloat sun_direction[] = {0.0, 0.0, -1.0, 0.0};
  // glLightfv(GL_LIGHT0, GL_POSITION, sun_direction);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();


  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  drawMesh();

  // not working :(
  //  // reset transformation for SFML
  //  // se:e https://www.sfml-dev.org/tutorials/2.5/window-opengl.php
  //  // Save the current OpenGL render states and matrices.
  //  // glPushAttrib(GL_ALL_ATTRIB_BITS);
  //  // glPushMatrix();
  // pushGLStates();
  //  // resetGLStates();
  // draw2DStack();
  //  // Restore the previously saved OpenGL render states and matrices.
  //  // glPopAttrib();
  //  // glPopMatrix();
  // popGLStates();


  sf::RenderWindow::display();
}

unsigned long SfmlRenderWindow::addMesh(const std::shared_ptr<BaseMesh>& simple_mesh) {
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

  for (const auto& mesh : meshes) {
    mesh.second->draw();
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
  window_ratio = static_cast<double>(sf::RenderWindow::getSize().x) /
                 sf::RenderWindow::getSize().y;
  camera.setAspectRatio(window_ratio);

  for (auto& mesh : meshes) {
    mesh.second->setProjection(camera.getProjectionMatrix());
  }
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

void SfmlRenderWindow::scrollHack(double f) { camera.ProcessMouseScroll(f); }
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

void SfmlRenderWindow::updateMeshesView() {
  for (auto& mesh : meshes) {
    mesh.second->setView(camera.getViewMatrix());
  }
}

void SfmlRenderWindow::updateMeshesPerspective() {
  for (auto& mesh : meshes) {
    mesh.second->setProjection(camera.getProjectionMatrix());
  }
}
void SfmlRenderWindow::setPerspective() {
  sf::RenderWindow::setActive(true);

  // Configure the viewport (the same size as the window)
  glViewport(0, 0, sf::RenderWindow::getSize().x, sf::RenderWindow::getSize().y);

  /*
  sf::View view;
  view.setSize(sf::RenderWindow::getSize().x, sf::RenderWindow::getSize().y);
  view.setCenter(sf::RenderWindow::getSize().x / 2.f,
                 sf::RenderWindow::getSize().y / 2.f);
  sf::RenderWindow::setView(view);
  */


  // projection without the use of a camera
  // glMatrixMode(GL_PROJECTION);
  // glLoadIdentity();

  //  const double bottom = -1.0;
  //  const double top = 1.0;
  //  const double left = -ratio;
  //  const double right = ratio;
  //  glFrustum(left, right, bottom, top, near_clipping, far_clipping);

  // perspective clipping like glm::perspective() or gluPerspective()
  // const double fH = tan(camera.lense_angle_rad) * camera.near_clipping;
  // const double fW = fH * window_ratio;
  // glFrustum(-fW, fW, -fH, fH, camera.near_clipping, camera.far_clipping);

  sf::RenderWindow::setActive(false);
}

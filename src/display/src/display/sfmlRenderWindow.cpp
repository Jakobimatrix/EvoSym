#include "sfmlRenderWindow.h"

#include <SFML/OpenGL.hpp>
#include <display_elements/glad_import.hpp>
#include <globals/globals.hpp>
#include <randomGenerator.hpp>
#include <utils/math.hpp>


SfmlRenderWindow::SfmlRenderWindow()
    : camera(Globals::getInstance().getPath2CameraSettings()),
      light(Globals::getInstance().getPath2LightSettings()) {
  Light::CallbackLightChange posChange = std::bind(&SfmlRenderWindow::onLightChange, this);
  light.setCallbackPositionChange(posChange);
}

void SfmlRenderWindow::init(const sf::WindowHandle& handle) {
  initOpenGl(handle);
  printGraphicCardInformation();
  initCamera();

  activateIf();
  // init mesh
  world_mesh = std::make_shared<WorldMesh>();
  // set perspective for world mesh
  const Eigen::Isometry3d pose = Eigen::Isometry3d::Identity();
  world_mesh->setPose(pose);
  unsigned int wmid = addMesh(world_mesh);

  const int nr_rand = 0;
  tool::RandomGenerator* rg;
  rg = &rg->getInstance();
  for (int i = 0; i < nr_rand; i++) {
    std::shared_ptr<WorldMesh> world_meshx = std::make_shared<WorldMesh>();
    Eigen::Vector3d rot(0, 0, 0);
    while (rot.norm() < 0.00001) {
      rot = Eigen::Vector3d(rg->uniformDistribution(-1, 1),
                            rg->uniformDistribution(-1, 1),
                            rg->uniformDistribution(-1, 1));
    }
    rot.normalize();
    world_meshx->setPose(eigen_utils::getTransformation(
        Eigen::Vector3d(rg->uniformDistribution(-15, 15),
                        rg->uniformDistribution(-15, 15),
                        rg->uniformDistribution(-15, 15)),
        rot));
    unsigned int wmidx = addMesh(world_meshx);
  }

  sun = std::make_shared<SunMesh>();
  Eigen::Isometry3d pose_sun;
  pose_sun.matrix() = light.getPose().matrix().cast<double>();
  sun->setPose(pose_sun);
  deactivateIf();

  unsigned int sid = addMesh(sun);
}

void SfmlRenderWindow::initOpenGl(const sf::WindowHandle& handle) {
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
  // Load OpenGL or OpenGL ES entry points using glad
  gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction));

  // kill bugs
  sf::RenderWindow::setActive(true);
  // SFML bug, first time calling isAvaiable it will deactivate the window
  // https://en.sfml-dev.org/forums/index.php?topic=17243.0
  sf::Shader::isAvailable();
  sf::RenderWindow::setActive(true);
  sf::Texture::getMaximumSize();

  enable3dDepth();

  updateWindowRatio();

  is_initialized = true;
}

void SfmlRenderWindow::initCamera() {
  setActive(true);

  Camera::CallbackCameraChange viewChange =
      std::bind(&SfmlRenderWindow::onCameraPositionUpdate, this);
  Camera::CallbackCameraChange perspectiveChange =
      std::bind(&SfmlRenderWindow::onCameraPerspectiveUpdate, this);

  camera.addCallbackPerspectiveChange(perspectiveChange);
  camera.addCallbackViewChange(viewChange);
  // camera.setPosition(0, 0, -20);

  camera.setAspectRatio(window_ratio);
  // camera.setLenseAngleRad(math::deg2Rad(30.));

  setActive(false);
}

void SfmlRenderWindow::printGraphicCardInformation() {
  activateIf();
  printf("Vendor graphic card: %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version GL: %s\n", glGetString(GL_VERSION));
  printf("Version GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  deactivateIf();
}

void SfmlRenderWindow::enable3dDepth() {
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(-1.0, 1.0);
}

void SfmlRenderWindow::update() {

  if (!sf::RenderWindow::isOpen() || !is_initialized) {
    return;
  }
  processInputActions();
  activateIf();

  // draw shadow shader

  // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawShadows();

  // draw scene
  glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  drawMesh();
  deactivateIf();

  // not working :(
  //  // reset transformation for SFML
  //  // se:e https://www.sfml-dev.org/tutorials/2.5/window-opengl.php
  //  // Save the current OpenGL render states and matrices.
  //  // glPushAttrib(GL_ALL_ATTRIB_BITS);
  //  // glPushMatrix();
  sf::RenderTarget::pushGLStates();
  //  // resetGLStates();
  draw2DStack();
  //  // Restore the previously saved OpenGL render states and matrices.
  //  // glPopAttrib();
  //  // glPopMatrix();
  sf::RenderTarget::popGLStates();


  sf::RenderWindow::display();
}

unsigned long SfmlRenderWindow::addMesh(const std::shared_ptr<BaseMesh>& simple_mesh) {
  meshes.emplace(std::make_pair(mesh_counter, simple_mesh));
  activateIf();
  simple_mesh->setProjection(camera.getProjectionMatrix());
  simple_mesh->setView(camera.getViewMatrix());
  simple_mesh->setCameraPosition(camera.getPosition());
  simple_mesh->setLight(light);
  deactivateIf();
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
  activateIf();
  for (const auto& mesh : meshes) {
    mesh.second->draw();
  }
  deactivateIf();
}

void SfmlRenderWindow::drawShadows() {

  activateIf();
  /*
   // todo this is part of simulation
   static Eigen::Vector3f nextPos = light.getPosition();
   if ((nextPos - light.getPosition()).norm() < 0.1f) {
     tool::RandomGenerator* rg;
     rg = &rg->getInstance();
     nextPos.x() = rg->uniformDistribution(-10, 10);
     nextPos.y() = rg->uniformDistribution(-10, 10);
     nextPos.z() = rg->uniformDistribution(-10, 10);
   } else {
     Eigen::Vector3f diff = nextPos - light.getPosition();
     diff.normalize();
     diff *= 0.03f;
     light.setPosition(light.getPosition() + diff);
     Eigen::Isometry3d pose_sun;
     pose_sun.matrix() = light.getPose().matrix().cast<double>();
     sun->setPose(pose_sun);
   }


     for (const auto& mesh : meshes) {
       mesh.second->draw(true);
     }
   */

  deactivateIf();
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
    processKeyPressedAction();
  }
}

void SfmlRenderWindow::updateWindowRatio() {

  window_ratio = static_cast<double>(sf::RenderWindow::getSize().x) /
                 sf::RenderWindow::getSize().y;
  setPerspective();
}

void SfmlRenderWindow::onResize() {
  updateWindowRatio();
  camera.setAspectRatio(window_ratio);
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

void SfmlRenderWindow::processKeyPressedAction() {
  Eigen::Vector3f move(0, 0, 0);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
    move.x() += 1;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
    move.x() -= 1;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
    move.y() += 1;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
    move.y() -= 1;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
    move.z() += 1;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
    move.z() -= 1;
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
    const bool debug_normals = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
    for (const auto& mesh : meshes) {
      mesh.second->setDebugNormals(debug_normals);
    }
  }

  move.normalize();
  const float length = 0.06f;
  move *= length;
  activateIf();
  light.setPositionAndTarget(light.getPosition() + move, Eigen::Vector3f(0, 0, 0));
  Eigen::Isometry3d pose_sun;
  pose_sun.matrix() =
      light.getPose().inverse(Eigen::TransformTraits::Isometry).matrix().cast<double>();
  sun->setPose(pose_sun);
  deactivateIf();
}

void SfmlRenderWindow::scrollHack(double f) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
    f *= 10;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
    camera.setLenseAngleRad(math::deg2Rad(f) + camera.getLenseAngleRad());
  } else {
    camera.shiftZ(f);
  }
}

void SfmlRenderWindow::dragMouseLeft(const sf::Vector2i& diff) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
    // FRE COMMAND SPACE
  } else {
    camera.rotatePY(diff.x, diff.y);
  }
}

void SfmlRenderWindow::dragMouseRight(const sf::Vector2i& diff) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
    camera.shiftXY(diff.x, diff.y);
  } else {
    camera.rotatePYaround(diff.x, diff.y, Eigen::Vector3d(0, 0, 0));
  }
}

void SfmlRenderWindow::leftKlick(const sf::Vector2i& mouse_pos) {
  F_DEBUG("klick left x: %d y: %d", mouse_pos.x, mouse_pos.y);
}

void SfmlRenderWindow::rightKlick(const sf::Vector2i& mouse_pos) {
  F_DEBUG("klick right x: %d y: %d", mouse_pos.x, mouse_pos.y);
}

void SfmlRenderWindow::onCameraPositionUpdate() {
  // todo rename its a pose change
  activateIf();
  for (auto& mesh : meshes) {
    mesh.second->setView(camera.getViewMatrix());
    mesh.second->setCameraPosition(camera.getPosition());
  }
  deactivateIf();
}

void SfmlRenderWindow::onCameraPerspectiveUpdate() {
  activateIf();
  for (auto& mesh : meshes) {
    mesh.second->setProjection(camera.getProjectionMatrix());
  }
  deactivateIf();
}

void SfmlRenderWindow::onLightChange() {
  activateIf();
  for (auto& mesh : meshes) {
    mesh.second->setLight(light);
  }
  deactivateIf();
}

void SfmlRenderWindow::setPerspective() {
  activateIf();

  // Configure the viewport (the same size as the window)
  glViewport(0, 0, sf::RenderWindow::getSize().x, sf::RenderWindow::getSize().y);

  /*
    sf::View view;
    view.setSize(sf::RenderWindow::getSize().x, sf::RenderWindow::getSize().y);
    view.setCenter(sf::RenderWindow::getSize().x / 2.f,
                   sf::RenderWindow::getSize().y / 2.f);
    sf::RenderWindow::setView(view);

    */

  // projection will be calculated in shader
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // model and view will be calculated in shader
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // open GL assumes now that every visible VErtex is within [-1,1]^3

  //  const double bottom = -1.0;
  //  const double top = 1.0;
  //  const double left = -ratio;
  //  const double right = ratio;
  //  glFrustum(left, right, bottom, top, near_clipping, far_clipping);

  // perspective clipping like glm::perspective() or gluPerspective()
  // const double fH = tan(camera.lense_angle_rad) * camera.near_clipping;
  // const double fW = fH * window_ratio;
  // glFrustum(-fW, fW, -fH, fH, camera.near_clipping, camera.far_clipping);

  deactivateIf();
}

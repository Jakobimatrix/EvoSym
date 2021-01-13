#include "renderWindow.h"

#include <SFML/OpenGL.hpp>
#include <globals/globals.hpp>
#include <randomGenerator.hpp>
#include <utils/math.hpp>


RenderWindow::RenderWindow()
    : camera(Globals::getInstance().getPath2CameraSettings()) {
  light_ptr = std::make_shared<Light>(Globals::getInstance().getPath2LightSettings());
  Light::CallbackLightChange posChange = std::bind(&RenderWindow::onLightChange, this);
  light_ptr->setCallbackPositionChange(posChange);
}

void RenderWindow::clean() {}

void RenderWindow::init() {
  initOpenGl();
  printGraphicCardInformation();
  initCamera();


  // init mesh
  world_mesh = std::make_shared<WorldMesh>();
  // set perspective for world mesh
  const Eigen::Isometry3d pose = Eigen::Isometry3d::Identity();
  world_mesh->setTransformMesh2World(pose);
  unsigned int wmid = addMesh(world_mesh);

  double dir[6][3] = {
      {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};

  double pos[6][3] = {
      {0, 1.5, 0}, {0, -1.5, 0}, {0, 3, 0}, {0, -3, 0}, {0, 4.5, 0}, {0, -4.5, 0}};

  double disp[8][3] = {{5, 5, 5},
                       {-5, 5, 5},
                       {5, -5, 5},
                       {5, 5, -5},
                       {-5, -5, 5},
                       {-5, 5, -5},
                       {5, -5, -5},
                       {-5, -5, -5}};

  for (int r = 0; r < 3; r++) {

    for (int j = 0; j < 8; j++) {
      for (int i = 0; i < 6; i++) {
        Eigen::Vector3d look_at(dir[i]);
        Eigen::Vector3d offset_local(pos[i]);
        Eigen::Vector3d offset_global(disp[j]);
        Eigen::Vector3d translation = offset_local * 1.2 + offset_global * 1.4;
        std::shared_ptr<WorldMesh> world_meshx = std::make_shared<WorldMesh>();
        world_meshx->setTransformMesh2World(
            eigen_utils::getTransformation(translation, look_at));
        unsigned int wmidx = addMesh(world_meshx);
      }
    }
    for (int j = 0; j < 6; j++) {
      std::rotate(pos[j], pos[j] + 1, pos[j] + 3);
    }
  }


  light_ptr->setShaddow();
}

void RenderWindow::initOpenGl() {
  QOpenGLFunctions::initializeOpenGLFunctions();
  setPerspective();
  enable3dDepth();

  is_initialized = true;
}

void RenderWindow::initCamera() {
  Camera::CallbackCameraChange viewChange =
      std::bind(&RenderWindow::onCameraPositionUpdate, this);
  Camera::CallbackCameraChange perspectiveChange =
      std::bind(&RenderWindow::onCameraPerspectiveUpdate, this);

  camera.addCallbackPerspectiveChange(perspectiveChange);
  camera.addCallbackViewChange(viewChange);
  // camera.setPosition(0, 0, -20);

  camera.setAspectRatio(window_ratio);
  // camera.setLenseAngleRad(math::deg2Rad(30.));
}

void RenderWindow::printGraphicCardInformation() {

  printf("Vendor graphic card: %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version GL: %s\n", glGetString(GL_VERSION));
  printf("Version GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void RenderWindow::enable3dDepth() {
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(-1.0, 1.0);
}

void RenderWindow::update() {

  // draw shadow shader
  glViewport(0, 0, 1024, 1024);  // TODO
  glCheck(glBindFramebuffer(GL_FRAMEBUFFER, light_ptr->getDepthMapFrameBufferInt()));
  glCheck(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
  glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  glCheck(glClearDepth(1));
  glCheck(glEnable(GL_DEPTH_TEST));
  glCheck(glDepthMask(GL_TRUE));
  drawShadows();
  glCheck(glBindFramebuffer(GL_FRAMEBUFFER, getDefualtFrameFuffer()));
  // glDrawBuffer(GL_BACK);  // default


  // draw scene
  //*
  glCheck(glViewport(0, 0, window_size.x(), window_size.y()));
  glCheck(glClearColor(0.3f, 0.3f, 0.3f, 0.0f));
  glCheck(glClearDepth(1));
  glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  glCheck(glEnable(GL_DEPTH_TEST));
  drawMesh();
  //*/

  /*
  // debug shadow/depth map
  glViewport(0, 0, window_size.x(), window_size.y());
  glCheck(glClear(GL_COLOR_BUFFER_BIT |
  GL_DEPTH_BUFFER_BIT)); glCheck(glDisable(GL_DEPTH_TEST));
  light_ptr->debugShadowTexture();
  */
}

unsigned long RenderWindow::addMesh(const std::shared_ptr<BaseMesh>& simple_mesh) {
  meshes.emplace(std::make_pair(mesh_counter, simple_mesh));

  simple_mesh->setProjection(camera.getProjectionMatrix());
  simple_mesh->setView(camera.getViewMatrix());
  simple_mesh->setCameraPosition(camera.getPosition());
  simple_mesh->setLight(light_ptr);

  return mesh_counter++;
}

bool RenderWindow::removeMesh(unsigned long id) {
  auto ptr = meshes.find(id);
  if (ptr != meshes.end()) {
    meshes.erase(ptr);
    return true;
  }
  return false;
}

void RenderWindow::drawMesh() {
  QOpenGLExtraFunctions* gl = QOpenGLContext::currentContext()->extraFunctions();
  for (const auto& mesh : meshes) {
    mesh.second->draw(gl);
  }
}

void RenderWindow::drawShadows() {


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
   */
  /*
   tool::RandomGenerator* rg;
   rg = &rg->getInstance();
   for (const auto& mesh : meshes) {
     const Eigen::Vector3d trans(rg->uniformDistribution(-0.01, 0.01),
                                 rg->uniformDistribution(-0.01, 0.01),
                                 rg->uniformDistribution(-0.01, 0.01));
     mesh.second->translate(trans);
     const Eigen::Vector3d rot(rg->uniformDistribution(-0.003, 0.003),
                               rg->uniformDistribution(-0.003, 0.003),
                               rg->uniformDistribution(-0.003, 0.003));
     mesh.second->rotate(rot);
   }

   */


  const double r = 150.5;
  static double phi = 0.;
  phi += 0.008;
  static double theta = 0.;
  theta += 0.008;
  static double y_dir = 1.;

  static double y = 0;
  y += 0.1 * y_dir;
  if (std::abs(y) > 100) {
    y_dir *= -1;
  }

  const double z = std::sin(phi) * r;
  const double x = std::cos(theta) * r;

  const Eigen::Vector3f light_pos(x, y, z);
  light_ptr->setPositionAndTarget(light_pos, Eigen::Vector3f(0, 0, 0));
  QOpenGLExtraFunctions* gl = QOpenGLContext::currentContext()->extraFunctions();

  for (const auto& mesh : meshes) {
    mesh.second->drawShadows(gl);
  }
}

void RenderWindow::onResize(int width, int height) {
  window_size.x() = width;
  window_size.y() = height;
  window_ratio = static_cast<double>(window_size.x()) / window_size.y();
  camera.setAspectRatio(window_ratio);
}

/*
void RenderWindow::processMouseAction() {

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


void RenderWindow::processKeyPressedAction() {
  /*
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


    move.normalize();
    const float length = 0.06f;
    move *= length;
    * /


if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
  const bool debug_normals = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
  for (const auto& mesh : meshes) {
    mesh.second->setDebugNormals(debug_normals);
  }
}
}
*/

void RenderWindow::scrollHack(double f) {
  /*
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
    f *= 10;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
    camera.setLenseAngleRad(math::deg2Rad(f) + camera.getLenseAngleRad());
  } else {
    camera.shiftZ(f);
  }
  */
}

void RenderWindow::dragMouseLeft(const Eigen::Vector2i& diff) {
  /*
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
      // FRE COMMAND SPACE
    } else {
      camera.rotatePY(diff.x, diff.y);
    }
    */
}

void RenderWindow::dragMouseRight(const Eigen::Vector2i& diff) {
  /*
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
      camera.shiftXY(diff.x, diff.y);
    } else {
      camera.rotatePYaround(diff.x, diff.y, Eigen::Vector3d(0, 0, 0));
    }
    */
}

void RenderWindow::leftKlick(const Eigen::Vector2i& mouse_pos) {
  F_DEBUG("klick left x: %d y: %d", mouse_pos.x(), mouse_pos.y());
}

void RenderWindow::rightKlick(const Eigen::Vector2i& mouse_pos) {
  F_DEBUG("klick right x: %d y: %d", mouse_pos.x(), mouse_pos.y());
}

void RenderWindow::onCameraPositionUpdate() {
  // todo rename its a pose change

  for (auto& mesh : meshes) {
    mesh.second->setView(camera.getViewMatrix());
    mesh.second->setCameraPosition(camera.getPosition());
  }
}

void RenderWindow::onCameraPerspectiveUpdate() {

  for (auto& mesh : meshes) {
    mesh.second->setProjection(camera.getProjectionMatrix());
  }
}

void RenderWindow::onLightChange() {

  for (auto& mesh : meshes) {
    mesh.second->setLight(light_ptr);
  }
}

void RenderWindow::setPerspective() {
  // projection will be calculated in shader
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // model and view will be calculated in shader
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

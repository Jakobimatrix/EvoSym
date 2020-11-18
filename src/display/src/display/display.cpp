#include "display.h"

// THIRD_PARTY_HEADERS_BEGIN
#include <glad/glad.h>
// THIRD_PARTY_HEADERS_END

void Display::onInit() {
  // Create the main window


  // create(sf::VideoMode(disp_width, disp_height, color_depth), Globals::getInstance().getMainVidowTitle(), sf::Style::Default);
  setVerticalSyncEnabled(true);
}

void Display::onUpdate() {
  processInputActions();
  if (!sf::RenderWindow::isOpen()) {
    return;
  }

  sf::RenderWindow::clear(sf::Color(0, 0, 0, 255));


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

unsigned long Display::addMesh(const disp_utils::MeshShaderPair& mesh_shader_pair) {
  meshes.emplace(std::make_pair(mesh_counter, mesh_shader_pair));
  return mesh_counter++;
}

bool Display::removeMesh(unsigned long id) {
  auto ptr = meshes.find(id);
  if (ptr != meshes.end()) {
    meshes.erase(ptr);
    return true;
  }
  return false;
}


void Display::draw2DStack() {
  // vector of boxes and text
}

void Display::drawMesh() {
  sf::RenderWindow::setActive(true);
  for (const auto& mesh_shader_pair : meshes) {
    // Mesh* m = mesh_shader_pair.second.first.get();
    // Shader* s = mesh_shader_pair.second.second.get();
    // m->Draw(*s);
    mesh_shader_pair.second.first.get()->Draw(*mesh_shader_pair.second.second.get());
  }
  sf::RenderWindow::setActive(false);
}

void Display::processInputActions() {
  sf::Event event;
  while (pollEvent(event)) {
    const sf::Vector2i mouse_pos =
        sf::Mouse::getPosition() + sf::RenderWindow::getPosition();

    // Close window: exit
    if (event.type == sf::Event::Closed) {
      sf::RenderWindow::close();
    }

    // Escape key: exit
    if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)) {
      sf::RenderWindow::close();
    }


    // Adjust the viewport when the window is resized
    if (event.type == sf::Event::Resized) {
      setPerspective();

      disp_height = sf::RenderWindow::getSize().y;
      disp_width = sf::RenderWindow::getSize().x;

      sf::View view;
      view.setSize(disp_width, disp_height);
      view.setCenter(disp_width / 2.f, disp_height / 2.f);
      sf::RenderWindow::setView(view);
    }

    last_mouse_pos = mouse_pos;
  }

  // process anny clicks
}

void Display::setPerspective() {
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

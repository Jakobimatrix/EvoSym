#include "display.h"

// TODO install properly https://askubuntu.com/questions/306703/compile-opengl-program-missing-gl-gl-h
#include "gl.h"

#define GLAD_GL_IMPLEMENTATION
#include "gl.h"

#ifdef SFML_SYSTEM_IOS
#include <SFML/Main.hpp>
#endif

#ifndef GL_SRGB8_ALPHA8
#define GL_SRGB8_ALPHA8 0x8C43
#endif


bool Display::initWindow() {
  // Create the main window
  window.create(sf::VideoMode(disp_width, disp_height, color_depth),
                Globals::getInstance().getMainVidowTitle(),
                sf::Style::Default);
  window.setVerticalSyncEnabled(true);

  return window.isOpen();
}

void Display::draw() {
  processInputActions();
  if (!window.isOpen()) {
    return;
  }

  // second draw triangles/Meshes
  drawMesh();

  // third reset transformation
  // Save the current OpenGL render states and matrices.
  window.pushGLStates();
  draw2DStack();
  // Restore the previously saved OpenGL render states and matrices.
  window.popGLStates();
  window.display();
}

void Display::draw2DStack() {
  drawMenu();

  // draw external menue and vector of boxes and text
}
void Display::drawMenu() {}



void Display::drawMesh() {
  window.setActive(true);

  window.setActive(false);
}

void Display::processInputActions() {
  sf::Event event;
  while (window.pollEvent(event)) {
    const sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);

    // Close window: exit
    if (event.type == sf::Event::Closed) {
      window.close();
    }

    // Escape key: exit
    if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)) {
      window.close();
    }


    // Adjust the viewport when the window is resized
    if (event.type == sf::Event::Resized) {
      setPerspective();

      disp_height = window.getSize().y;
      disp_width = window.getSize().x;

      sf::View view;
      view.setSize(disp_width, disp_height);
      view.setCenter(disp_width / 2.f, disp_height / 2.f);
      window.setView(view);
    }

    last_mouse_pos = mouse_pos;
  }

  // process anny clicks
}

void Display::setPerspective() {
  window.setActive(true);

  // Load OpenGL or OpenGL ES entry points using glad
#ifdef SFML_OPENGL_ES
  gladLoadGLES1(reinterpret_cast<GLADloadfunc>(sf::Context::getFunction));
#else
  gladLoadGL(reinterpret_cast<GLADloadfunc>(sf::Context::getFunction));
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
  glViewport(0, 0, window.getSize().x, window.getSize().y);
  ///


  // Setup a perspective projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
#ifdef SFML_OPENGL_ES
  GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
  glFrustumf(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);
#else
  GLdouble ratio = static_cast<double>(window.getSize().x) / window.getSize().y;
  glFrustum(-ratio, ratio, -1., 1., 1., 500.);
#endif

  window.setActive(false);
}

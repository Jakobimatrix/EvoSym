#ifndef OPEN_GL_WIDGET
#define OPEN_GL_WIDGET

#include <qtimer.h>

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include "renderWindow.h"

class OpenGLWidget : public QOpenGLWidget, public RenderWindow {
  Q_OBJECT

 public:
  explicit OpenGLWidget(QWidget *parent);

  void scrollHack(double f);

  ~OpenGLWidget() override {}

 protected:
  // Your widget's OpenGL rendering context is made current when paintGL(), resizeGL(), or initializeGL() is called. If you need to call the standard OpenGL API functions from other places (e.g. in your widget's constructor or in your own paint functions), you must call makeCurrent() first.
  // Sets up the OpenGL resources and state. Gets called once before the first time resizeGL() or paintGL() is called.
  void initializeGL(void) override;

  // Sets up the OpenGL viewport, projection, etc. Gets called whenever the widget has been resized (and also when it is shown for the first time because all newly created widgets get a resize event automatically).
  void resizeGL(int width, int height) override;

  // Renders the OpenGL scene. Gets called whenever the widget needs to be updated.
  void paintGL(void) override;

 protected slots:
  void release(void);

 private:
  // We make the paintEvent function return a null paint engine.
  // This functions works together with the WA_PaintOnScreen flag
  // to tell Qt that we're not using any of its built-in paint engines.
  QPaintEngine *paintEngine() const override { return nullptr; }
  void showEvent(QShowEvent *) override;
  void closeEvent(QCloseEvent *event) override;
  void focusInEvent(QFocusEvent *event) override;
  void focusOutEvent(QFocusEvent *event) override;
  void enterEvent(QEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void moveEvent(QMoveEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  bool setKeyStatus(int key, bool set);
  void processKeyEvent(int key);

  // User input
  tool::SingleTimer mouse_left_timer;
  tool::SingleTimer mouse_right_timer;
  static constexpr std::chrono::milliseconds MAX_KLICK_DURATION =
      std::chrono::milliseconds(300);
  Eigen::Vector2i last_mouse_pos = Eigen::Vector2i(0, 0);
  bool mouse_over = false;
};

#endif

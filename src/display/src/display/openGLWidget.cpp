#include "openGLWidget.h"

#include <QMouseEvent>
#include <QOpenGLFramebufferObject>

OpenGLWidget::OpenGLWidget(QWidget* parent, unsigned int fps)
    : QOpenGLWidget(parent) {
  setFramerate(fps);
}

void OpenGLWidget::initializeGL() {
  RenderWindow::CallbackGetDefaultFrameBuffer func =
      std::bind(&OpenGLWidget::defaultFramebufferObject, this);
  RenderWindow::setDefaultFrameBufferGetter(func);
  RenderWindow::init();

  connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(release()));

  // connect(&timer, SIGNAL(timeout()), this, SLOT(repaint()));
  // QWidget::update() function does not cause an immediate repaint; instead it schedules a paint event for processing when Qt returns to the main event loop.
  connect(&timer, SIGNAL(timeout()), this, SLOT(QWidget::update()));
  timer.start();
}

void OpenGLWidget::release() {
  WARNING("release signal received.");
  RenderWindow::clean();
}

void OpenGLWidget::paintGL(void) {
  RenderWindow::update();
  QWidget::update();
}

void OpenGLWidget::resizeGL(int width, int height) {
  RenderWindow::onResize(width, height);
}

void OpenGLWidget::setFramerate(unsigned int fps) {
  const double update_rate_ms = 1000. / fps;
  const int update_rate_ms_i = static_cast<int>(std::round(update_rate_ms));
  timer.setInterval(update_rate_ms_i);
}

void OpenGLWidget::showEvent(QShowEvent* event) {
  WARNING("showEvent triggered.");
  QOpenGLWidget::showEvent(event);
}
void OpenGLWidget::closeEvent(QCloseEvent* event) {
  WARNING("closeEvent triggered.");
  QOpenGLWidget::closeEvent(event);
}
void OpenGLWidget::focusInEvent(QFocusEvent* event) {
  WARNING("focusInEvent triggered.");
  QOpenGLWidget::focusInEvent(event);
}
void OpenGLWidget::focusOutEvent(QFocusEvent* event) {
  WARNING("focusOutEvent triggered.");
  QOpenGLWidget::focusOutEvent(event);
}
void OpenGLWidget::enterEvent(QEvent* event) {
  WARNING("enterEvent triggered.");
  QOpenGLWidget::enterEvent(event);
}
void OpenGLWidget::keyPressEvent(QKeyEvent* event) {
  WARNING("keyPressEvent triggered.");
  QOpenGLWidget::keyPressEvent(event);
}
void OpenGLWidget::keyReleaseEvent(QKeyEvent* event) {
  WARNING("keyReleaseEvent triggered.");
  QOpenGLWidget::keyReleaseEvent(event);
}
void OpenGLWidget::leaveEvent(QEvent* event) {
  WARNING("leaveEvent triggered.");
  QOpenGLWidget::leaveEvent(event);
}
void OpenGLWidget::mouseDoubleClickEvent(QMouseEvent* event) {
  WARNING("mouseDoubleClickEvent triggered.");
  QOpenGLWidget::mouseDoubleClickEvent(event);
}
void OpenGLWidget::mouseMoveEvent(QMouseEvent* event) {
  WARNING("mouseMoveEvent triggered.");
  QOpenGLWidget::mouseMoveEvent(event);
}
void OpenGLWidget::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::RightButton) {
    return;
  }
  WARNING("mouseMoveEvent triggered.");
  QOpenGLWidget::mousePressEvent(event);

  // m_lastPosition = event->pos( );
}
void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event) {
  WARNING("mouseReleaseEvent triggered.");
  QOpenGLWidget::mouseReleaseEvent(event);
}
void OpenGLWidget::moveEvent(QMoveEvent* event) {
  WARNING("moveEvent triggered.");
  QOpenGLWidget::moveEvent(event);
}

void OpenGLWidget::paintEvent(QPaintEvent* event) {
  // WARNING("paintEvent triggered.");
  QOpenGLWidget::paintEvent(event);
}
void OpenGLWidget::resizeEvent(QResizeEvent* event) {
  WARNING("resizeEvent triggered.");
  QOpenGLWidget::resizeEvent(event);
}

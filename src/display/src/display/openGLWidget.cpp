#include "openGLWidget.h"

#include <QMouseEvent>
#include <QOpenGLFramebufferObject>

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);
}

void OpenGLWidget::initializeGL() {
  RenderWindow::CallbackGetDefaultFrameBuffer func =
      std::bind(&OpenGLWidget::defaultFramebufferObject, this);
  RenderWindow::setDefaultFrameBufferGetter(func);
  RenderWindow::init();

  connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(release()));

  // connect(&timer, SIGNAL(timeout()), this, SLOT(repaint()));
  // QWidget::update() function does not cause an immediate repaint; instead it schedules a paint event for processing when Qt returns to the main event loop.
  // connect(&timer, SIGNAL(timeout()), this, SLOT(QWidget::update()));
  // TODO timer not needed
  // timer.start();
}

void OpenGLWidget::scrollHack(double f) {
  if (hasFocus() && mouse_over) {
    RenderWindow::scroll(f);
  }
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

void OpenGLWidget::showEvent(QShowEvent* event) {
  DEBUG("showEvent triggered.");
  QOpenGLWidget::showEvent(event);
}

void OpenGLWidget::closeEvent(QCloseEvent* event) {
  DEBUG("closeEvent triggered.");
  QOpenGLWidget::closeEvent(event);
}

void OpenGLWidget::focusInEvent(QFocusEvent* event) {
  // window in foreground
  QOpenGLWidget::focusInEvent(event);
}

void OpenGLWidget::focusOutEvent(QFocusEvent* event) {
  // window in background
  QOpenGLWidget::focusOutEvent(event);
}

void OpenGLWidget::enterEvent(QEvent* event) {
  mouse_over = true;
  QOpenGLWidget::enterEvent(event);
}

void OpenGLWidget::keyPressEvent(QKeyEvent* event) {
  if (!event->isAutoRepeat()) {
    if (!setKeyStatus(event->key(), true)) {
      processKeyEvent(event->key());
    }
  }
  QOpenGLWidget::keyPressEvent(event);
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent* event) {
  // always set key == false e.g: key pressed, mouse drags, mouse leaves window, drag stopps, leave event is triggered, key is released.
  setKeyStatus(event->key(), false);
  QOpenGLWidget::keyReleaseEvent(event);
}

void OpenGLWidget::leaveEvent(QEvent* event) {
  mouse_over = false;
  QOpenGLWidget::leaveEvent(event);
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event) {
  const Eigen::Vector2i mouse_pos(event->x(), event->y());
  const Eigen::Vector2i mouse_diff = mouse_pos - last_mouse_pos;
  last_mouse_pos = mouse_pos;
  if (mouse_left_timer.hasStarted()) {
    RenderWindow::dragMouseLeft(mouse_diff);
  } else if (mouse_right_timer.hasStarted()) {
    RenderWindow::dragMouseRight(mouse_diff);
  }
  QOpenGLWidget::mouseMoveEvent(event);
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event) {
  last_mouse_pos << event->x(), event->y();
  if (event->button() == Qt::RightButton) {
    if (!mouse_left_timer.hasStarted()) {
      mouse_right_timer.start();
    }
    is_pressed.mouse_right = true;
  } else if (event->button() == Qt::LeftButton) {
    if (!mouse_right_timer.hasStarted()) {
      mouse_left_timer.start();
    }
    is_pressed.mouse_left = true;
  } else if (event->button() == Qt::MiddleButton) {
    is_pressed.mouse_mid = true;
  }
  QOpenGLWidget::mousePressEvent(event);
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::RightButton) {
    if (mouse_right_timer.hasStarted()) {
      if (MAX_KLICK_DURATION > mouse_right_timer.getPassedTime<std::chrono::milliseconds>()) {
        const Eigen::Vector2i pos(event->screenPos().x(), event->screenPos().y());
        rightKlick(pos);
      }
      mouse_right_timer.stop();
    }
    is_pressed.mouse_right = false;
  } else if (event->button() == Qt::LeftButton) {
    if (mouse_left_timer.hasStarted()) {
      if (MAX_KLICK_DURATION > mouse_left_timer.getPassedTime<std::chrono::milliseconds>()) {
        const Eigen::Vector2i pos(event->screenPos().x(), event->screenPos().y());
        leftKlick(pos);
      }
      mouse_left_timer.stop();
    }
    is_pressed.mouse_left = false;
  } else if (event->button() == Qt::MiddleButton) {
    is_pressed.mouse_mid = false;
  }
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

bool OpenGLWidget::setKeyStatus(int key, bool set) {
  switch (key) {
    case Qt::Key_Escape:
      is_pressed.esc = set;
      break;
    case Qt::Key_Control:
      is_pressed.ctrl = set;
      break;
    case Qt::Key_Alt:
      is_pressed.alt = set;
      break;
    case Qt::Key_AltGr:
      is_pressed.alt_gr = set;
      break;
    case Qt::Key_Space:
      is_pressed.space = set;
      break;
    case Qt::Key_Shift:
      is_pressed.shift = set;
      break;
    default:
      return false;
      break;
  }
  return true;
}

void OpenGLWidget::processKeyEvent(int key) {
  switch (key) {
    case Qt::Key_N:
      keyN();
      break;
    default:
      break;
  }
}

#include "qSfmlCanvas.h"

QSfmlCanvas::QSfmlCanvas(QWidget* Parent, unsigned int fps)
    : QOpenGLWidget(Parent) {
  // Setup some states to allow direct rendering into the widget
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_OpaquePaintEvent);
  setAttribute(Qt::WA_NoSystemBackground);
  // setSurfaceType(QSurface::OpenGLSurface);

  // Set strong focus to enable keyboard events to be received
  setFocusPolicy(Qt::StrongFocus);

  // Setup the widget geometry
  move(QPoint(0, 0));
  resize(Parent->size());

  // Setup the timer
  setFramerate(fps);
}

void QSfmlCanvas::setFramerate(unsigned int fps) {
  const double update_rate_ms = 1000. / fps;
  const int update_rate_ms_i = static_cast<int>(std::round(update_rate_ms));
  timer.setInterval(update_rate_ms_i);
}

void QSfmlCanvas::showEvent(QShowEvent*) {
  if (!show_event_triggered) {
    // Under X11, we need to flush the commands sent to the server to ensure
    // that SFML will get an updated view of the windows
#ifdef Q_WS_X11
    XFlush(QX11Info::display());
#endif

    // init sfml
    init(winId());

    // Setup the timer to trigger a refresh at specified framerate
    connect(&timer, SIGNAL(timeout()), this, SLOT(repaint()));
    timer.start();

    show_event_triggered = true;
  }
}

void QSfmlCanvas::resizeEvent(QResizeEvent* event) {
  if (SfmlRenderWindow::isInitialized()) {
    // Needed since for some reason SFML canvas does not get
    // resized when QWidged holding that canvas gets resized.
    // So we set new size manually.
    const sf::Vector2u new_size(QWidget::geometry().width(), QWidget::geometry().height());
    // const sf::Vector2i new_pos(QWidget::geometry().x(), QWidget::geometry().y());
    sf::RenderWindow::setSize(new_size);
    // sf::RenderWindow::setPosition(new_pos);
  }
  QWidget::resizeEvent(event);
}

/*
 Building a QT Widged capable of showing the sfml window as suggested here:
 https://www.sfml-dev.org/tutorials/1.6/graphics-qt.php
 and here
 http://johnmarinelli.github.io/notes/qt-sfml
*/

#ifndef QSFMLCANVAS
#define QSFMLCANVAS

#include <qtimer.h>
#include <qwidget.h>

#include <SFML/Graphics.hpp>
#include <cmath>

#ifdef Q_WS_X11
#include <Qt/qx11info_x11.h>
#include <X11/Xlib.h>
#endif

class QSFMLCanvas : public QWidget, public sf::RenderWindow {
 public:
  QSFMLCanvas(QWidget* Parent, unsigned int fps = 25)
      : QWidget(Parent), myInitialized(false) {
    // Setup some states to allow direct rendering into the widget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // Set strong focus to enable keyboard events to be received
    setFocusPolicy(Qt::StrongFocus);

    // Setup the widget geometry
    move(QPoint(0, 0));
    resize(Parent->size());

    // Setup the timer
    setFramerate(fps);
  }

  void setFramerate(unsigned int fps) {
    const double update_rate_ms = 1000. / fps;
    const int update_rate_ms_i = static_cast<int>(std::round(update_rate_ms));
    myTimer.setInterval(update_rate_ms_i);
  }


  ~QSFMLCanvas() {}

 protected:
  void resizeEvent(QResizeEvent* event) override {
    // move(QPoint(0, 0));
    // fill parent
    resize(parentWidget()->size());
    QWidget::resizeEvent(event);
  }

 private:
  virtual void onInit() = 0;

  virtual void onUpdate() = 0;

  // We make the paintEvent function return a null paint engine.
  // This functions works together with the WA_PaintOnScreen flag
  // to tell Qt that we're not using any of its built-in paint engines.
  QPaintEngine* paintEngine() const override { return nullptr; }

  void showEvent(QShowEvent*) override {
    if (!myInitialized) {
      // Under X11, we need to flush the commands sent to the server to ensure
      // that SFML will get an updated view of the windows
#ifdef Q_WS_X11
      XFlush(QX11Info::display());
#endif

      // Create the SFML window with the widget handle
      sf::RenderWindow::create(winId());

      // Let the derived class do its specific stuff
      onInit();

      // Setup the timer to trigger a refresh at specified framerate
      connect(&myTimer, SIGNAL(timeout()), this, SLOT(repaint()));
      myTimer.start();

      myInitialized = true;
    }
  }

  void paintEvent(QPaintEvent*) override {
    // Let the derived class do its specific stuff
    onUpdate();
  }

  QTimer myTimer;
  bool myInitialized;
};

#endif

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
#include <iostream>

#include <SFML/Graphics.hpp>

#ifdef Q_WS_X11
#include <Qt/qx11info_x11.h>
#include <X11/Xlib.h>
#endif

class QSFMLCanvas : public QWidget, public sf::RenderWindow {
 public:
  QSFMLCanvas(QWidget* Parent, unsigned int FrameTime = 0)
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
    myTimer.setInterval(static_cast<int>(FrameTime));
  }


  ~QSFMLCanvas() {}

 protected:
  void resizeEvent(QResizeEvent* event) override {
  // TODO NOT CALLED IF PARENT CHANGES
    move(QPoint(0, 0));
    resize(size());
    QWidget::resizeEvent(event);
    printf("resize Event\n");
  }

  void moveEvent(QMoveEvent *event) override {
   // TODO NOT CALLED IF PARENT CHANGES
    move(QPoint(0, 0));
    resize(size());
    QWidget::moveEvent(event);
    printf("move Event\n");
  }

 private:
  virtual void onInit() = 0;

  virtual void onUpdate() = 0;

  // We make the paintEvent function return a null paint engine.
  // This functions works together with the WA_PaintOnScreen flag
  // to tell Qt that we're not using any of its built-in paint engines.
  QPaintEngine* paintEngine() const { return nullptr; }

  void showEvent(QShowEvent*) {
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

  void paintEvent(QPaintEvent*) {
    // Let the derived class do its specific stuff
    onUpdate();

    // Display on screen
    sf::RenderWindow::display();
  }

  QTimer myTimer;
  bool myInitialized;
};

#endif

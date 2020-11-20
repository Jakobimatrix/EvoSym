#ifndef SQ_SFML_CANVAS
#define SQ_SFML_CANVAS

#include <qtimer.h>
#include <qwidget.h>

#include "sfmlRenderWindow.h"

class QSfmlCanvas : public QWidget, public SfmlRenderWindow {
 Q_OBJECT
 public:
  explicit QSfmlCanvas(QWidget* Parent, unsigned int fps = 25);

  void setFramerate(unsigned int fps);

  ~QSfmlCanvas() override {}

 private:
  // We make the paintEvent function return a null paint engine.
  // This functions works together with the WA_PaintOnScreen flag
  // to tell Qt that we're not using any of its built-in paint engines.
  QPaintEngine* paintEngine() const override { return nullptr; }

  void showEvent(QShowEvent*) override;

  void paintEvent(QPaintEvent*) override { SfmlRenderWindow::update(); }
  bool show_event_triggered = false;
  QTimer timer;
};

#endif

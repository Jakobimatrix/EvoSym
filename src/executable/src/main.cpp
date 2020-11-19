#include <display/mainWindow.h>

#include <QApplication>
#include <globals/globals.hpp>

int main(int argc, char *argv[]) {

  QApplication app(argc, argv);
  app.setApplicationName(Globals::getInstance().getMainVidowTitle().c_str());
  DisplayQt mainWin;
#if defined(Q_OS_SYMBIAN)
  mainWin.showMaximized();
#else
  mainWin.show();
#endif
  return app.exec();
}

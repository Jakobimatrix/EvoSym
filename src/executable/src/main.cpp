#include <display/displayQt.h>

#include <QApplication>
#include <QSurfaceFormat>
#include <globals/globals.hpp>
#include <locale>

int main(int argc, char* argv[]) {

  // make sure to always use the same decimal point separator
  std::locale("C");

  QApplication app(argc, argv);
  app.setApplicationName(Globals::getInstance().getMainVidowTitle().c_str());
  DisplayQt mainWin;
  QSurfaceFormat format;
  format.setDepthBufferSize(24);
  format.setStencilBufferSize(8);
  format.setVersion(3, 0);
  format.setProfile(QSurfaceFormat::CompatibilityProfile);
  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
  QSurfaceFormat::setDefaultFormat(format);
#if defined(Q_OS_SYMBIAN)
  mainWin.showMaximized();
#else
  mainWin.show();
#endif
  return app.exec();
}

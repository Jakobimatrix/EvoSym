#include <display/displayQt.h>

#include <QApplication>
#include <QSurfaceFormat>
#include <globals/globals.hpp>

int main(int argc, char* argv[]) {

  QApplication app(argc, argv);
  app.setApplicationName(Globals::getInstance().getMainVidowTitle().c_str());
  DisplayQt mainWin;
  QSurfaceFormat format;
  format.setDepthBufferSize(24);
  format.setStencilBufferSize(8);
  format.setVersion(3, 0);
  format.setProfile(QSurfaceFormat::CompatibilityProfile);
  format.setRenderableType(QSurfaceFormat::OpenGL);
  QSurfaceFormat::setDefaultFormat(format);
#if defined(Q_OS_SYMBIAN)
  mainWin.showMaximized();
#else
  mainWin.show();
#endif
  return app.exec();
}

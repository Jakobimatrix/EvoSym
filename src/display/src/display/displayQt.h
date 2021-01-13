// shamelessly copied from https://doc.qt.io/archives/qt-4.8/qt-mainwindows-application-example.html
#ifndef DISPLAY_QT
#define DISPLAY_QT

#include <QMainWindow>

#include "display.h"
#include "openGLWidget.h"

class DisplayQt : public QMainWindow, public Display {
  Q_OBJECT

 public:
  DisplayQt();

  ~DisplayQt() override;

 protected:
  void closeEvent(QCloseEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void moveEvent(QMoveEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

 private:
  void setStatus(const std::string &msg, int timeout = 0) override;
  void setStatus(const QString &msg, int timeout = 0);

  bool askYesNoQuestion(const std::string &question,
                        const std::string &title = "") override;

  void popup_info(const std::string &text, const std::string &title = "") override;

  void popup_warning(const std::string &text, const std::string &title = "") override;

  void popup_error(const std::string &text, const std::string &title = "") override;

  void setWindowFilePath(const std::string &file_path) override;


 private slots:
  bool save();
  bool saveAs();
  void about() override;
  void newWorld();
  void open();
  void close();

 private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();

  // QT stuff

  OpenGLWidget *open_gl_widget;

  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *helpMenu;
  QToolBar *fileToolBar;
  QToolBar *editToolBar;
  QAction *newAct;
  QAction *openAct;
  QAction *saveAct;
  QAction *saveAsAct;
  QAction *exitAct;
  QAction *aboutAct;
};


#endif

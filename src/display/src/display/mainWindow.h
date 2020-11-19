// shamelessly copied from https://doc.qt.io/archives/qt-4.8/qt-mainwindows-application-example.html
#ifndef MAINWINDOW
#define MAINWINDOW

#include <world/world.h>

#include <QMainWindow>
#include <thread>

#include "sfmlView.h"
#include "simulationSettings.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow();

 protected:
  void closeEvent(QCloseEvent *event) override;

 private:
  void runSimulation();

  bool simulationIsRunning();

  void pauseSimulation();

  void stopSimulation();

  bool startSimulation();

  void setStatus(const std::string &msg, int timeout = 0);
  void setStatus(const char *str, int timeout = 0);
  void setStatus(const QString &msg, int timeout = 0);


 private slots:
  bool save();
  bool saveAs();
  void about();
  void newWorld();
  void open();
  void exitGracefully();

 private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void readSettings();
  void writeSettings();
  bool askSave();
  void loadFile(const QString &fileName);
  void setCurrentFile(const QString &fileName);
  QString strippedName(const QString &fullFileName);
  void initSimulation();

  // QT stuff

  SFMLView *sfml_view;

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

  // Simulation
  QString current_world_file;
  SimulationSettings simulationSettings;
  World simulatedWorld;

  const std::string DEFAULT_FILE_NAME = std::string("new_world.evsm");

  std::thread simulation_thread;
  bool pause_simulation = false;
  bool stop_simulation = false;
};


#endif

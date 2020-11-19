#include "mainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QtWidgets>
#include <globals/globals.hpp>
#include <globals/macros.hpp>

MainWindow::MainWindow() {
  current_world_file = QString(DEFAULT_FILE_NAME.c_str());
  sfml_view = new SFMLView(this);
  setCentralWidget(sfml_view);

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  readSettings();

  setUnifiedTitleAndToolBarOnMac(true);
}


void MainWindow::closeEvent(QCloseEvent *event) {
  if (askSave()) {
    // writeSettings();
    // event->accept();
  } else {
    // event->ignore();
  }
  WARNING("TODO");
}

void MainWindow::setStatus(const std::string &msg, int timeout) {
  setStatus(msg.c_str(), timeout);
}
void MainWindow::setStatus(const char *str, int timeout) {
  statusBar()->showMessage(tr(str), timeout);
}
void MainWindow::setStatus(const QString &msg, int timeout) {
  setStatus(msg.toStdString(), timeout);
}

bool MainWindow::save() {
  const bool is_simulating = simulationIsRunning();
  if (is_simulating) {
    stopSimulation();
  }

  setStatus("Save " + current_world_file.toStdString());
  const bool save_success = simulatedWorld.save(current_world_file.toStdString());
  if (is_simulating) {
    startSimulation();
  }
  return save_success;
}

bool MainWindow::saveAs() {
  setStatus("Save As ...");
  QString file_name = QFileDialog::getSaveFileName(
      this, tr("Save current Simulation as"), current_world_file, tr("Address Book (*.abk);;All Files (*)"));

  if (file_name.isEmpty())
    return false;

  setCurrentFile(file_name);
  return save();
}

void MainWindow::loadFile(const QString &fileName) {
  setStatus("Loading " + fileName.toStdString() + " ...");
  WARNING("TODO");
  setCurrentFile(fileName);
}

bool MainWindow::askSave() {
  WARNING("TODO");
  QMessageBox::StandardButton ret;
  ret = QMessageBox::warning(this,
                             tr("Application"),
                             tr("Do you want to save your changes?"),
                             QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  if (ret == QMessageBox::Save)
    return save();
  else if (ret == QMessageBox::Cancel)
    return false;
  return false;
}


void MainWindow::setCurrentFile(const QString &file_name) {
  if (file_name.isEmpty()) {
    current_world_file = QString(DEFAULT_FILE_NAME.c_str());
  } else {
    current_world_file = file_name;
  }

  setWindowModified(false);
  setWindowFilePath(current_world_file);
}

QString MainWindow::strippedName(const QString &fullFileName) {
  return QFileInfo(fullFileName).fileName();
}


void MainWindow::newWorld() { WARNING("TODO"); }

void MainWindow::open() { WARNING("TODO"); }


void MainWindow::about() {
  WARNING("TODO");
  QMessageBox::about(this,
                     tr("About EvoSym"),
                     tr("Here <b>could</b> be your advertisement "
                        "or here, "
                        "but not here."));
}


void MainWindow::createActions() {
  // new wolrd
  const std::string path = Globals::getInstance().getAbsPath2Resources();
  newAct = new QAction(QIcon((path + "new.png").c_str()), tr("&New"), this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("Create a new file."));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newWorld()));

  // open world
  openAct = new QAction(QIcon((path + "open.png").c_str()), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing file."));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  // about
  aboutAct = new QAction(tr("About &EvoSym"), this);
  aboutAct->setStatusTip(tr("Show the EvoSym's About box."));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  // save world
  saveAct = new QAction(QIcon((path + "save.png").c_str()), tr("&Save Current World"), this);
  saveAct->setStatusTip(tr("Save the current simulation progress."));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  // saveAsAct world
  saveAsAct = new QAction(tr("Save Current World &As"), this);
  saveAsAct->setStatusTip(
      tr("Save the current simulation progress as a new simulation. (Dont "
         "overwrite old file)"));
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  // exit
  exitAct = new QAction(tr("&Exit"), this);
  exitAct->setStatusTip(tr("Close the program."));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(exitGracefully()));
}

void MainWindow::createMenus() {
  WARNING("TODO");
  fileMenu = menuBar()->addMenu(tr("&File"));
  if (fileMenu == nullptr) {
    ERROR("filemenue is nullptr :(");
    return;
  }
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  //  editMenu->addAction(cutAct);
  //  editMenu->addAction(copyAct);
  //  editMenu->addAction(pasteAct);

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutAct);
}

void MainWindow::initSimulation() {
  WARNING("TODO");
  // read file
  simulatedWorld.init();
  unsigned long mash_id = sfml_view->addMesh(simulatedWorld.getWorldsMeshShaderPair());
}

void MainWindow::createToolBars() {
  WARNING("TODO");
  fileToolBar = addToolBar(tr("File"));
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(openAct);
  fileToolBar->addAction(saveAct);

  editToolBar = addToolBar(tr("Edit"));
  //  editToolBar->addAction(cutAct);
  //  editToolBar->addAction(copyAct);
  //  editToolBar->addAction(pasteAct);
}

void MainWindow::createStatusBar() {
  statusBar()->showMessage(tr("Ready"));
  QColor backGroundColor = palette().color(QPalette::Window);
  const std::string backgroud_style_sheet =
      "background-color: " + backGroundColor.name().toStdString() + ";";
  statusBar()->setStyleSheet(backgroud_style_sheet.c_str());
}

void MainWindow::readSettings() {
  WARNING("TODO");
  QSettings settings("Trolltech", "Application Example");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();
  resize(size);
  move(pos);
}

void MainWindow::writeSettings() {
  WARNING("TODO");
  QSettings settings("Trolltech", "Application Example");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

void MainWindow::exitGracefully() { WARNING("TODO: Signal missing"); }

bool MainWindow::startSimulation() {
  if (simulationIsRunning()) {
    return false;
  }
  stop_simulation = false;
  simulation_thread = std::thread(&MainWindow::runSimulation, this);
  setStatus("Run simulation ...");
  return true;
}

void MainWindow::stopSimulation() {
  if (simulationIsRunning()) {
    setStatus("Stopping simulation ...");
    stop_simulation = true;
    simulation_thread.join();
    stop_simulation = false;
    setStatus("Stopping stoppend.");
  }
}

void MainWindow::runSimulation() {
  while (sfml_view->isOpen() && !stop_simulation) {
    simulatedWorld.update();
  }
}

bool MainWindow::simulationIsRunning() { return simulation_thread.joinable(); }

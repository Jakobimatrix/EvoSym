#include "mainWindow.h"

#include <QMessageBox>
#include <QtWidgets>
#include <globals/globals.hpp>
#include <globals/macros.hpp>

MainWindow::MainWindow() {

  SFMLView = new Display(this);
  setCentralWidget(SFMLView);

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  readSettings();

  setCurrentFile("");
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


bool MainWindow::save() {
  //  if (curFile.isEmpty()) {
  //    return saveAs();
  //  } else {
  //    return saveFile(curFile);
  //  }
  return false;
}

bool MainWindow::saveAs() {
  WARNING("TODO");
  return false;
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
  WARNING("TODO");

  // new wolrd
  const std::string path = Globals::getInstance().getAbsPath2Resources();
  newAct = new QAction(QIcon((path + "new.png").c_str()), tr("&New"), this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("Create a new file"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newWorld()));

  // open world
  openAct = new QAction(QIcon((path + "open.png").c_str()), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  // about
  aboutAct = new QAction(tr("About &EvoSym"), this);
  aboutAct->setStatusTip(tr("Show the EvoSym's About box"));
  connect(aboutAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  // save world
  saveAct = new QAction(tr("Save &current &world"), this);
  saveAct->setStatusTip(tr("Safe the current simulation progress"));
  WARNING("TODO: Signal missing");

  // saveAsAct world
  saveAsAct = new QAction(tr("Save &current &world as"), this);
  saveAsAct->setStatusTip(
      tr("Safe the current simulation progress as a new simulation. (dont "
         "overwrite old)"));
  WARNING("TODO: Signal missing");


  // exit
  exitAct = new QAction(tr("Save &current &world"), this);
  exitAct->setStatusTip(tr("Safe the current simulation progress"));
  WARNING("TODO: Signal missing");
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
  // simulatedWorld.init();
  int mash_id = SFMLView->addMesh(simulatedWorld.getWorldMesh());
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
  WARNING("TODO");
  statusBar()->showMessage(tr("Ready"));
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


void MainWindow::loadFile(const QString &fileName) {
  WARNING("TODO");
  setCurrentFile(fileName);
  statusBar()->showMessage(tr("File loaded"), 2000);
}


bool MainWindow::saveFile(const QString &fileName) {
  WARNING("TODO");
  setCurrentFile(fileName);
  statusBar()->showMessage(tr("File saved"), 2000);
  return true;
}

void MainWindow::setCurrentFile(const QString &fileName) {
  WARNING("TODO");
  curFile = fileName;
  setWindowModified(false);

  QString shownName = curFile;
  if (curFile.isEmpty())
    shownName = "untitled.txt";
  setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName) {
  return QFileInfo(fullFileName).fileName();
}


void MainWindow::runSimulation() {
  WARNING("TODO");
  while (SFMLView->isOpen()) {
    simulatedWorld.update();
  }
  // save
}

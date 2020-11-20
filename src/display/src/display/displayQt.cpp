#include "displayQt.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QtWidgets>
#include <globals/globals.hpp>
#include <globals/macros.hpp>

DisplayQt::DisplayQt() {
  sfml_view = new QSfmlCanvas(this);
  setCentralWidget(sfml_view);

  QPoint qpos(getDisplayPosX(), getDisplayPosY());
  QSize qsize(getDisplaySizeW(), getDisplaySizeH());
  resize(qsize);
  move(qpos);

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  setUnifiedTitleAndToolBarOnMac(true);
}

DisplayQt::~DisplayQt() {}

void DisplayQt::close() { QMainWindow::close(); }

void DisplayQt::closeEvent(QCloseEvent *event) {
  if (!exitGracefully()) {
    event->ignore();
  }
  event->accept();
}

void DisplayQt::resizeEvent(QResizeEvent *event) {
  saveDisplaySize(event->size().width(), event->size().height());
  QMainWindow::resizeEvent(event);
}

void DisplayQt::moveEvent(QMoveEvent *event) {
  saveDisplayPosition(event->pos().x(), event->pos().y());
  QMainWindow::moveEvent(event);
}

bool DisplayQt::askYesNoQuestion(const std::string &question, const std::string &title) {
  QMessageBox::StandardButton ret;
  ret = QMessageBox::question(
      this, tr(title.c_str()), tr(question.c_str()), QMessageBox::Yes | QMessageBox::No);
  if (ret == QMessageBox::Yes)
    return true;
  return false;
}

void DisplayQt::popup_info(const std::string &text, const std::string &title) {
  QMessageBox::information(this, tr(text.c_str()), tr(title.c_str()), QMessageBox::Ok);
}

void DisplayQt::popup_warning(const std::string &text, const std::string &title) {
  QMessageBox::warning(this, tr(text.c_str()), tr(title.c_str()), QMessageBox::Ok);
}

void DisplayQt::popup_error(const std::string &text, const std::string &title) {
  QMessageBox::critical(this, tr(text.c_str()), tr(title.c_str()), QMessageBox::Ok);
}

void DisplayQt::setWindowFilePath(const std::string &file_path) {
  const QString path(file_path.c_str());
  QMainWindow::setWindowFilePath(path);
}

void DisplayQt::setStatus(const std::string &msg, int timeout) {
  statusBar()->showMessage(tr(msg.c_str()), timeout);
}
void DisplayQt::setStatus(const QString &msg, int timeout) {
  setStatus(msg.toStdString(), timeout);
}

bool DisplayQt::save() { return Display::save(); }

bool DisplayQt::saveAs() {
  QString file_name =
      QFileDialog::getSaveFileName(this,
                                   tr("Save current Simulation as"),
                                   tr(getCurrentFileName().c_str()),
                                   tr("Address Book (*.abk);;All Files (*)"));

  return Display::saveAs(file_name.toStdString());
}


void DisplayQt::newWorld() { WARNING("TODO"); }

void DisplayQt::open() { WARNING("TODO"); }


void DisplayQt::about() {
  WARNING("TODO");
  QMessageBox::about(this,
                     tr("About EvoSym"),
                     tr("Here <b>could</b> be your advertisement "
                        "or here, "
                        "but not here."));
}


void DisplayQt::createActions() {
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
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void DisplayQt::createMenus() {
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

void DisplayQt::createToolBars() {
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

void DisplayQt::createStatusBar() {
  statusBar()->showMessage(tr("Ready"));
  QColor backGroundColor = palette().color(QPalette::Window);
  const std::string backgroud_style_sheet =
      "background-color: " + backGroundColor.name().toStdString() + ";";
  statusBar()->setStyleSheet(backgroud_style_sheet.c_str());
}

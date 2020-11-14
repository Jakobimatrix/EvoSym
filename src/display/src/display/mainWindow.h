// shamelessly copied from https://doc.qt.io/archives/qt-4.8/qt-mainwindows-application-example.html
#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>
#include "display.h"
#include "simulationSettings.h"
#include <world/world.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void runSimulation();

private slots:
    bool save();
    bool saveAs();
    void about();
    void newWorld();
    void open();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool askSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void initSimulation();

    // QT stuff
    QString curFile;

    Display *SFMLView;

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
    SimulationSettings simulationSettings;
    World simulatedWorld;
};


#endif

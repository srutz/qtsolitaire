#include "mainwindow.h"
#include "solitairewidget.h"
#include <QMenuBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    auto centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);

    auto layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    centralWidget->setLayout(layout);

    auto solitaireWidget = new SolitaireWidget(centralWidget);
    layout->addWidget(solitaireWidget);

    // add menubar and file exit menu
    auto menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    auto menuFile = menuBar->addMenu(tr("&File"));
    auto actionExit = menuFile->addAction(tr("E&xit"));
    connect(actionExit, &QAction::triggered, this, &QMainWindow::close);

    auto actionDumpGameState = menuFile->addAction(tr("&Dump Game State"));
    connect(actionDumpGameState, &QAction::triggered, [solitaireWidget]() { solitaireWidget->game().state().dump(); });
}

void MainWindow::resizeEvent(QResizeEvent *event) { QMainWindow::resizeEvent(event); }

MainWindow::~MainWindow() {}

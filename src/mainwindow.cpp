#include "mainwindow.h"
#include "solitairewidget.h"
#include <QFile>
#include <QMenuBar>
#include <QStandardPaths>
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
    auto menuEdit = menuBar->addMenu(tr("&Edit"));

    auto actionNewGame = menuFile->addAction(tr("&New Game"));
    actionNewGame->setShortcut(QKeySequence::New);
    connect(actionNewGame, &QAction::triggered, [solitaireWidget]() {
        solitaireWidget->game().resetGame();
        solitaireWidget->layoutGame();
    });
    menuFile->addSeparator();

    auto actionLoadGameState = menuFile->addAction(tr("&Load Game State"));
    connect(actionLoadGameState, &QAction::triggered, [solitaireWidget]() {
        solitaireWidget->game().loadFromFile();
        solitaireWidget->layoutGame();
    });
    auto actionSaveGameState = menuFile->addAction(tr("&Save Game State"));
    connect(actionSaveGameState, &QAction::triggered, [solitaireWidget]() { solitaireWidget->game().saveToFile(); });

    // menuFile->addSeparator();
    // auto actionDumpGameState = menuFile->addAction(tr("&Dump Game State"));
    // connect(actionDumpGameState, &QAction::triggered, [solitaireWidget]() { solitaireWidget->game().state().dump(); });

    menuFile->addSeparator();
    auto actionExit = menuFile->addAction(tr("E&xit"));
    actionExit->setShortcut(QKeySequence::Quit);
    connect(actionExit, &QAction::triggered, this, &QMainWindow::close);

    auto actionUndo = menuEdit->addAction(tr("&Undo"));
    actionUndo->setShortcut(QKeySequence::Undo);
    connect(actionUndo, &QAction::triggered, [solitaireWidget]() {
        solitaireWidget->game().undo();
        solitaireWidget->layoutGame();
    });

    auto actionRedo = menuEdit->addAction(tr("&Redo"));
    actionRedo->setShortcut(QKeySequence::Redo);
    connect(actionRedo, &QAction::triggered, [solitaireWidget]() {
        solitaireWidget->game().redo();
        solitaireWidget->layoutGame();
    });

    const auto &game = solitaireWidget->game();
    connect(&game, &Game::undoRedoStateChanged, [actionUndo, actionRedo, &game]() {
        actionUndo->setEnabled(game.canUndo());
        actionRedo->setEnabled(game.canRedo());
    });
    actionUndo->setEnabled(game.canUndo());
    actionRedo->setEnabled(game.canRedo());
}

void MainWindow::resizeEvent(QResizeEvent *event) { QMainWindow::resizeEvent(event); }

MainWindow::~MainWindow() {}

#include "mainwindow.h"
#include "backgroundwork.h"
#include "gamestartview.h"
#include "solitairewidget.h"
#include <QFile>
#include <QMenuBar>
#include <QStandardPaths>
#include <QThread>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    auto *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);

    auto *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    centralWidget->setLayout(layout);

    auto *stackContainer = new QWidget(centralWidget);
    layout->addWidget(stackContainer);

    m_stackedLayout = new QStackedLayout(stackContainer);

    auto *game = new Game(this);

    auto *gameStartView = new GameStartView(stackContainer);
    m_stackedLayout->addWidget(gameStartView);

    auto *solitaireWidget = new SolitaireWidget(game, centralWidget);
    m_stackedLayout->addWidget(solitaireWidget);

    // add menubar and file exit menu
    auto *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    auto *menuFile = menuBar->addMenu(tr("&File"));
    auto *menuEdit = menuBar->addMenu(tr("&Edit"));

    auto *actionNewGame = menuFile->addAction(tr("&New Game"));
    actionNewGame->setShortcut(QKeySequence::New);
    connect(actionNewGame, &QAction::triggered, [game, solitaireWidget]() {
        if (game->mode() == GameMode::STOPPED) {
            game->setMode(GameMode::RUNNING);
        }
        game->resetGame();
        solitaireWidget->layoutGame(true);
    });
    menuFile->addSeparator();

    auto *actionLoadGameState = menuFile->addAction(tr("&Load Game State"));
    connect(actionLoadGameState, &QAction::triggered, [game, solitaireWidget]() {
        game->loadFromFile();
        solitaireWidget->layoutGame();
    });
    auto *actionSaveGameState = menuFile->addAction(tr("&Save Game State"));
    connect(actionSaveGameState, &QAction::triggered, [game, solitaireWidget]() { game->saveToFile(); });

    // menuFile->addSeparator();
    // auto actionDumpGameState = menuFile->addAction(tr("&Dump Game State"));
    // connect(actionDumpGameState, &QAction::triggered, [solitaireWidget]() { solitaireWidget->game().state().dump(); });

    menuFile->addSeparator();
    auto *actionExit = menuFile->addAction(tr("E&xit"));
    actionExit->setShortcut(QKeySequence::Quit);
    connect(actionExit, &QAction::triggered, this, &QMainWindow::close);

    auto *actionUndo = menuEdit->addAction(tr("&Undo"));
    actionUndo->setShortcut(QKeySequence::Undo);
    connect(actionUndo, &QAction::triggered, [game, solitaireWidget]() {
        game->undo();
        solitaireWidget->layoutGame();
    });

    auto *actionRedo = menuEdit->addAction(tr("&Redo"));
    actionRedo->setShortcut(QKeySequence::Redo);
    connect(actionRedo, &QAction::triggered, [game, solitaireWidget]() {
        game->redo();
        solitaireWidget->layoutGame();
    });

    connect(game, &Game::undoRedoStateChanged, [actionUndo, actionRedo, game]() {
        actionUndo->setEnabled(game->canUndo());
        actionRedo->setEnabled(game->canRedo());
    });
    actionUndo->setEnabled(game->canUndo());
    actionRedo->setEnabled(game->canRedo());

    connect(game, &Game::modeChanged, [this, game, solitaireWidget, gameStartView]() {
        if (game->mode() == RUNNING) {
            m_stackedLayout->setCurrentWidget(solitaireWidget);
            solitaireWidget->layoutGame(true);
        } else {
            m_stackedLayout->setCurrentWidget(gameStartView);
        }
    });
}

void MainWindow::resizeEvent(QResizeEvent *event) { QMainWindow::resizeEvent(event); }

MainWindow::~MainWindow() {}

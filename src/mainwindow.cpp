#include "mainwindow.h"
#include "solitairewidget.h"
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
}

void MainWindow::resizeEvent(QResizeEvent *event) { QMainWindow::resizeEvent(event); }

MainWindow::~MainWindow() {}

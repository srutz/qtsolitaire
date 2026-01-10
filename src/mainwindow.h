#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  protected:
    void resizeEvent(QResizeEvent *event) override;

  private:
    QStackedLayout *m_stackedLayout;
};
#endif // MAINWINDOW_H

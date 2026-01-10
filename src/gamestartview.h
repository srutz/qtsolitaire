
#ifndef GAMESTARTVIEW_H
#define GAMESTARTVIEW_H

#include <QWidget>

class GameStartView : public QWidget
{
    Q_OBJECT

  public:
    GameStartView(QWidget *parent = nullptr);
    ~GameStartView() = default;
};

#endif /* GAMESTARTVIEW_H */

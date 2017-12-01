#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

struct VecStruct {
    int ix,iy;
    double f,g,h;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    QPainter *painter;
    bool mousepressed;
    QList<int> myX,myY;
    QList<VecStruct> openlist,closelist,pathList;
    VecStruct start,goal;
    void AStar();
    int place[24][32];
    void openPos(int i, int j, int);
    double guessMove(int i, int j);
    bool isGoal();
    int findMin();
    void keyPressEvent(QKeyEvent *e);
    void Reset_AStar();
    void backTrace();
};

#endif // MAINWINDOW_H

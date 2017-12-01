#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPaintEvent>
#include <QPainter>
#include <QMessageBox>
#include <QDebug>
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    painter = new QPainter();
    mousepressed = false;

    Reset_AStar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    painter->begin(this);
//    painter->setRenderHint(QPainter::Antialiasing, true);

    // Open List
    painter->setBrush(QBrush(Qt::yellow));
    for(int i=0; i<openlist.length(); i++) {
        painter->setPen(QPen(Qt::yellow, 1));
        painter->drawRect(openlist.at(i).ix*25,openlist.at(i).iy*25,25,25);
        painter->setPen(QPen(Qt::black, 0.25));
        painter->drawText(openlist.at(i).ix*25,openlist.at(i).iy*25+12,QString::number(openlist.at(i).f,'g',2));
    }
    // Closed List
    painter->setBrush(QBrush(Qt::gray));
    for(int i=0; i<closelist.length(); i++) {
        painter->setPen(QPen(Qt::gray, 1));
        painter->drawRect(closelist.at(i).ix*25,closelist.at(i).iy*25,25,25);
        painter->setPen(QPen(Qt::black, 0.25));
        painter->drawText(closelist.at(i).ix*25+1,closelist.at(i).iy*25+12,QString::number(closelist.at(i).f,'g',2));
    }
    // Path List
    painter->setBrush(QBrush(Qt::black));
    for(int i=0; i<pathList.length(); i++) {
        painter->setPen(QPen(Qt::black, 1));
        painter->drawRect(pathList.at(i).ix*25,pathList.at(i).iy*25,25,25);
    }

    if(mousepressed==true) {
        painter->setBrush(QBrush(Qt::blue));
        painter->setPen(QPen(Qt::blue, 1));
        for(int i=0; i<myX.length(); i++)
            painter->drawRect(myX.at(i),myY.at(i),25,25);
    }

    painter->setBrush(QBrush(Qt::green));
    painter->setPen(QPen(Qt::green, 1));
    painter->drawRect(start.ix*25,start.iy*25,25,25);
    painter->setBrush(QBrush(Qt::red));
    painter->setPen(QPen(Qt::red, 1));
    painter->drawRect( goal.ix*25, goal.iy*25,25,25);

    painter->setPen(QPen(Qt::black, 0.5));
    for(int i=25; i<800; i+=25)
        painter->drawLine(i,0,i,600);
    for(int i=25; i<600; i+=25)
        painter->drawLine(0,i,800,i);

    painter->end();
}

void MainWindow::mousePressEvent(QMouseEvent * e)
{
    if(e->button()==Qt::LeftButton) {
        QPoint p = e->pos();
        myX.append(p.x() - (p.x()%25));
        myY.append(p.y() - (p.y()%25));
        int rX = p.x()/25;
        int rY = p.y()/25;
        place[rY][rX] = 55;
        mousepressed = true;
        update();
//        QMessageBox::warning(this, tr("My Application"),str,QMessageBox::Ok);
    }
}

void MainWindow::keyPressEvent(QKeyEvent * e) {
    if(e->key() == Qt::Key_S) {
        AStar();
        update();
    }
    if(e->key() == Qt::Key_R) {
        Reset_AStar();
        update();
    }
    if(e->key() == Qt::Key_P) {
        if(isGoal()==false) {
            QMessageBox::warning(this, tr("A Star"),"No Path",QMessageBox::Ok);
        } else {
        backTrace();
        update();
        }
    }
}

void MainWindow::Reset_AStar() {
    myX.clear();
    myY.clear();
    openlist.clear();
    closelist.clear();
    pathList.clear();

    start.ix=0;
    start.iy=0;
    start.g=0;

    goal.ix=31;
    goal.iy=23;

    closelist.append(start);

    for(int i=0; i<24; i++)
        for(int j=0; j<32; j++)
            place[i][j] = 99; // For Free
    place[0][0] = 11; // For Start
    place[0][0] = 22; // For Goal
    // 33 => For Open
    // 44 => For Closed
    // 55 => For Block
}

void MainWindow::AStar() {


    openPos(start.iy,start.ix,start.g);

    VecStruct temp;
    while(openlist.length() != 0 && isGoal()==false) {
        int min = findMin();
        if(min == -1)
            temp = openlist.takeLast();
        else
            temp = openlist.takeAt(min);
        openPos(temp.iy,temp.ix,temp.g);
        closelist.append(temp);
    }
    int len = closelist.length();
    temp = closelist.at(len-1);
    goal.g = temp.g;
    closelist.append(goal);
//    backTrace();
//    qDebug() << temp.f;
}

int MainWindow::findMin() {
    double min1 = openlist.at(0).f;
    int index = 0;
    for(int i=1; i<openlist.length(); i++) {
        if(openlist.at(i).f < min1) {
            index = i;
            min1 = openlist.at(i).f;
        }
    }
//    if(min2 == min1)
//        return -1;
    return index;
}

bool MainWindow::isGoal() {
    for(int i=0; i<openlist.length(); i++) {
        if(openlist.at(i).ix == goal.ix &&
                openlist.at(i).iy == goal.iy)
            return true;
    }
    return false;
}



void MainWindow::openPos(int y, int x, int g) {
    VecStruct temp;
    if(x>0) {
        if(place[y][x-1]==99) {
            temp.iy = y;
            temp.ix = x-1;
            temp.h = guessMove(y,x-1);
            temp.g = g+1;
            temp.f = temp.h + temp.g;
            openlist.append(temp);
            place[y][x-1]=33;
//            qDebug() << openlist.length() << endl;
        }
    }
    if(y>0) {
        if(place[y-1][x]==99) {
            temp.iy = y-1;
            temp.ix = x;
            temp.h = guessMove(y-1,x);
            temp.g = g+1;
            temp.f = temp.h + temp.g;
            openlist.append(temp);
            place[y-1][x]=33;
//            qDebug() << openlist.length() << endl;
        }
    }
    if(x<31) {
        if(place[y][x+1]==99) {
            temp.iy = y;
            temp.ix = x+1;
            temp.h = guessMove(y,x+1);
            temp.g = g+1;
            temp.f = temp.h + temp.g;
            openlist.append(temp);
            place[y][x+1]=33;
//            qDebug() << openlist.length() << endl;
        }
    }
    if(y<23) {
        if(place[y+1][x]==99) {
            temp.iy = y+1;
            temp.ix = x;
            temp.h = guessMove(y+1,x);
            temp.g = g+1;
            temp.f = temp.h + temp.g;
            openlist.append(temp);
            place[y+1][x]=33;
//            qDebug() << openlist.length() << endl;
        }
    }
}

double MainWindow::guessMove(int i, int j) {
    int temp = 0;
    int tx = j;
    int ty = i;
    while(ty<goal.iy) {
        ty++;
        temp++;
    }
    while(tx<goal.ix) {
        tx++;
        temp++;
    }

    return sqrt(pow(abs(j - goal.ix), 2) + pow(abs(i - goal.iy), 2));
//    return temp;
}

void MainWindow::backTrace() {
    int len = closelist.length();
    len--;
    double n = closelist.at(len).g;
    int x = closelist.at(len).ix;
    int y = closelist.at(len).iy;
    while(n>0) {
        len--;
        if(closelist.at(len).g==n
                && abs(closelist.at(len).ix-x)<=1
                && abs(closelist.at(len).iy-y)<=1) {
            x = closelist.at(len).ix;
            y = closelist.at(len).iy;
            pathList.append(closelist.at(len));
            n--;
        }
    }
}


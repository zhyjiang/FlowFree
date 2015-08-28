#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QMediaPlayer>
#include "point.h"

#define NOTACTIVE 10

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *);
    void clearPoint(Point *);
    bool isWin();
    void pipeCheck();

public slots:
    void setLevel(int);
    void start();
    void leave();
    void bgmPlay();

private slots:
    void nextLevel();
    void previousLevel();
    void toStart();
    void ReStart();
    void autoSolve();
    void help();

signals:
    void backToStart();
    void moveChange(int);
    void pipeChange(double);
    void levelChange(int);

private:
    void findPath(int);
    void writeInfo();
    void readInfo();

    Ui::MainWindow *ui;
    Point m_point[7][7];
    QPoint mousePosition;
    QMediaPlayer m_break;
    QMediaPlayer m_bgm;

    bool m_noMore;
    bool m_win;
    int m_level;
    int m_colorNum;
    int m_origins[18][2];
    int m_finishedLevel[100];
    bool m_rectFlag[9];
    int m_chosenLevel;
    enum {easy, normal, hard};
    enum {blank, red, blue, purple, yellow, orange, green, black, brown};
    int m_height;
    int m_widht;
    int active_X, active_Y;
    int m_move;
    int m_totalLevel;
    int m_path[10][50][2];
    int m_pathLength[10];
};

#endif // MAINWINDOW_H

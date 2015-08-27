#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QSound>
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
    void paintEvent(QPaintEvent *);
    void clearPoint(Point *);
    bool isWin();
    void pipeCheck();

public slots:
    void setLevel(int);
    void start();
    void leave();

private slots:
    void nextLevel();
    void previousLevel();
    void toStart();
    void ReStart();
    void autoSolve();

signals:
    void backToStart();
    void moveChange(int);
    void pipeChange(double);

private:
    void findPath(int, int, int);

    Ui::MainWindow *ui;
    Point m_point[7][7];
    QPoint mousePosition;
    QSound m_break;

    bool m_start;
    bool m_noMore;
    bool m_win;
    int m_level;
    int m_colorNum;
    int m_origins[18][2];
    bool m_rectFlag[9];
    int m_chosenLevel;
    enum {easy, normal, hard};
    enum {blank, red, blue, purple, yellow, orange, green, black, brown};
    int m_height;
    int m_widht;
    int active_X, active_Y;
    int m_move;
};

#endif // MAINWINDOW_H
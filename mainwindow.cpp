#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"
#include <QtGui>
#include <QString>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFile>
#include <string>
#include <cmath>
#include <ctime>

namespace Resource{
QColor color[9] = {QColor(237, 28, 36), QColor(0, 162, 232), QColor(102, 24, 126),
                   QColor(244, 233, 11), QColor(255, 127, 39), QColor(144, 233, 50),
                   QColor(0, 0, 0), QColor(185, 122, 87), QColor(254, 109, 221)};
QColor activeColor[9] = {QColor(237, 28, 36, 100), QColor(0, 162, 232, 100),
                         QColor(102, 24, 126, 100), QColor(244, 233, 11, 100),
                         QColor(255, 127, 39, 100), QColor(144, 233, 50, 100),
                         QColor(0, 0, 0, 100), QColor(185, 122, 87, 100),
                         QColor(254, 109, 221, 100)};
int stepX[4] = {-1, 0, 1, 0};
int stepY[4] = {0, -1, 0, 1};
}

using Resource::color;
using Resource::activeColor;
using Resource::stepX;
using Resource::stepY;

QString setLevelNum(int);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mousePosition(0, 0),
    m_timer(this)
{
    ui->setupUi(this);
    m_chosenLevel = 1;
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    m_break.setMedia(QUrl::fromLocalFile("/Sound/break.mp3"));
    m_bgm.setMedia(QUrl::fromLocalFile("/Sound/bgm.mp3"));
    m_bgm.setVolume(50);
    m_bgm.play();
    m_timer.setInterval(1000);
    m_timer.start();

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeIsOut()));
    connect(ui->Next_Level, SIGNAL(triggered()), this, SLOT(nextLevel()));
    connect(ui->Previous_Level, SIGNAL(triggered()), this, SLOT(previousLevel()));
    connect(ui->Back_to_Start, SIGNAL(triggered()), this, SLOT(toStart()));
    connect(ui->ReStart, SIGNAL(triggered()), this, SLOT(ReStart()));
    connect(ui->next, SIGNAL(clicked()), this, SLOT(nextLevel()));
    connect(ui->back, SIGNAL(clicked()), this, SLOT(previousLevel()));
    connect(ui->reStart, SIGNAL(clicked()), this, SLOT(ReStart()));
    connect(ui->actionAutoSolve, SIGNAL(triggered()), this, SLOT(autoSolve()));
    connect(ui->autoSolve, SIGNAL(clicked(bool)), this, SLOT(autoSolve()));
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(help()));
    connect(this, SIGNAL(levelChange(int)), ui->lcdNumber_3, SLOT(display(int)));
    connect(this, SIGNAL(moveChange(int)), ui->lcdNumber, SLOT(display(int)));
    connect(this, SIGNAL(pipeChange(double)), ui->lcdNumber_2, SLOT(display(double)));
    QTimer::singleShot(245000, this, SLOT(bgmPlay()));

    QFile file(setLevelNum(m_chosenLevel));
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream txtInput(&file);
    txtInput >> m_level >> m_colorNum;

    for(int i = 0; i < m_colorNum * 2; ++i)
    {
        txtInput >> m_origins[i][0] >> m_origins[i][1];
        m_point[m_origins[i][0]][m_origins[i][1]].color[0] = i/2+1;
        m_point[m_origins[i][0]][m_origins[i][1]].color[1] = i/2+1;
        m_point[m_origins[i][0]][m_origins[i][1]].origin = true;
    }
    file.close();

    active_X = NOTACTIVE;
    active_Y = NOTACTIVE;
    m_move = 0;
    m_noMore = false;
    m_mode = false;
    m_totalLevel = 90;

    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
            m_point[i][j].setPosition(i, j);

    for(int i = 0; i < 9; ++i)
        m_rectFlag[i] = true;

    readInfo();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start(bool mode)
{
    setEnabled(true);
    m_mode = mode;
    timeOver = 0;
    show();
    pipeCheck();
}

void MainWindow::leave()
{
    close();
}

void MainWindow::setLevel(int level)
{
    m_chosenLevel = level;
    if(m_chosenLevel < 1)
        m_chosenLevel = 1;
    else if(m_chosenLevel == 91)
        m_chosenLevel = 90;

    QFile file(setLevelNum(m_chosenLevel));
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(this, "Wrong", "Can not find the level.", QMessageBox::Ok);
        m_chosenLevel = 1;
        file.setFileName(setLevelNum(m_chosenLevel));
        file.open(QIODevice::ReadOnly | QIODevice::Text);
    }

    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
        {
            m_point[i][j].clear();
            m_point[i][j].origin = false;
        }

    QTextStream txtInput(&file);
    txtInput >> m_level >> m_colorNum;
    for(int i = 0; i < m_colorNum * 2; ++i)
    {
        txtInput >> m_origins[i][0] >> m_origins[i][1];
        m_point[m_origins[i][0]][m_origins[i][1]].color[0] = i/2+1;
        m_point[m_origins[i][0]][m_origins[i][1]].color[1] = i/2+1;
        m_point[m_origins[i][0]][m_origins[i][1]].origin = true;
    }
    file.close();
    active_X = NOTACTIVE;
    active_Y = NOTACTIVE;
    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
            m_point[i][j].setPosition(i, j);

    m_widht = 190 + 70 * (m_level + 5);
    m_height = 100 + 70 * (m_level + 5) + 20;
    resize(m_widht, m_height);

    move((QApplication::desktop()->width() - (50 + 70 * (m_level + 5)))/2,
         (QApplication::desktop()->height() - (50 + 70 * (m_level + 5) + 50))/2);

    m_move = 0;
    emit moveChange(m_move);
    emit levelChange(m_chosenLevel);

    pipeCheck();
    timeOver = 0;

    update();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for (int i = 1; i < m_level + 5; ++i)
    {
        painter.drawLine(95, 45 + i * 70, m_widht - 95, 45 + i * 70);
        painter.drawLine(95 + i * 70, 45, 95 + i * 70, m_height - 75);
    }

    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
        {
            if(m_point[i][j].color[1] != blank && m_point[i][j].rectFlag && m_rectFlag[m_point[i][j].color[1]])
            {
                painter.setPen(activeColor[m_point[i][j].color[1]-1]);
                painter.setBrush(activeColor[m_point[i][j].color[1]-1]);
                painter.drawRect(95 + 70 * i, 45 + 70 * j, 70, 70);
            }
        }

    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
        {
            if(m_point[i][j].color[0] != blank && m_point[i][j].origin)
            {
                painter.setPen(color[m_point[i][j].color[0]-1]);
                painter.setBrush(color[m_point[i][j].color[0]-1]);
                painter.drawEllipse(QPoint(130 + 70 * i, 80 + 70 * j), 20, 20);
            }
            if(m_point[i][j].previous[0] != 0 && !m_point[i][j].covered)
            {
                painter.setPen(QPen(QBrush(color[m_point[i][j].color[0]-1]),
                        15, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painter.setBrush(color[m_point[i][j].color[0]-1]);
                painter.drawLine(QPoint(130 + 70 * m_point[i][j].previous[0]->x, 80 + 70 * m_point[i][j].previous[0]->y),
                                 QPoint(130 + 70 * i, 80 + 70 * j));
            }
            if(m_point[i][j].previous[1])
            {
                painter.setPen(QPen(QBrush(color[m_point[i][j].color[0]-1]),
                        15, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painter.setBrush(color[m_point[i][j].color[0]-1]);
                painter.drawLine(QPoint(130 + 70 * m_point[i][j].previous[1]->x, 80 + 70 * m_point[i][j].previous[1]->y),
                                 QPoint(130 + 70 * i, 80 + 70 * j));
            }
        }

    if(active_X != NOTACTIVE)
    {
        if(m_point[active_X][active_Y].color[0] != blank)
        {
            painter.setPen(activeColor[m_point[active_X][active_Y].color[0]-1]);
            painter.setBrush(activeColor[m_point[active_X][active_Y].color[0]-1]);
            painter.drawEllipse(mousePosition, 40, 40);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *mouseEvent)
{
    if(mouseEvent->x() > 95 && mouseEvent->y() > 25 && mouseEvent->x() < m_widht - 95
            && mouseEvent->y() < m_height - 75)
    {
        int x = floor((mouseEvent->x()-95) / 70);
        int y = floor((mouseEvent->y()-45) / 70);
        active_X = x;
        active_Y = y;
        m_noMore = false;
        mousePosition.setX(mouseEvent->x());
        mousePosition.setY(mouseEvent->y());
        if(m_point[x][y].color[1] != blank)
            m_rectFlag[m_point[x][y].color[1]] = false;
        if(m_point[x][y].color[1] != blank && m_point[x][y].next[0] != 0)
        {
            Point *tempPoint = m_point[x][y].next[0];
            m_point[x][y].next[0] = 0;
            m_point[x][y].rectFlag = true;
            clearPoint(tempPoint);
        }
        if(m_point[x][y].origin)
        {
            for (int i = 0; i < m_level + 5; ++i)
                for (int j = 0; j < m_level + 5; ++j)
                {
                    if(m_point[i][j].color[0] == m_point[x][y].color[0] && !m_point[i][j].origin)
                        m_point[i][j].clear();
                    if(m_point[i][j].origin && m_point[i][j].color[0] == m_point[x][y].color[0])
                    {
                        m_point[i][j].next[0] = 0;
                        m_point[i][j].previous[0] = 0;
                        m_point[i][j].rectFlag = false;
                    }
                }
            m_point[x][y].rectFlag = true;
        }
    }
    pipeCheck();
    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if(mouseEvent->x() > 95 && mouseEvent->y() > 25 && mouseEvent->x() < m_widht - 95
            && mouseEvent->y() < m_height - 75)
    {
        int x = floor((mouseEvent->x()-95) / 70);
        int y = floor((mouseEvent->y()-45) / 70);

        mousePosition.setX(mouseEvent->x());
        mousePosition.setY(mouseEvent->y());
        if(mouseEvent->buttons() & Qt::LeftButton && active_X != NOTACTIVE &&
                abs(active_X - x) + abs(active_Y - y) == 1 && m_point[active_X][active_Y].next[1] == 0 &&
                    m_point[active_X][active_Y].color[0] != blank)
        {
            int temp = m_point[active_X][active_Y].previous[1]?1:0;
            if(m_point[x][y].origin != true &&
                    m_point[x][y].color[0] == blank && !m_noMore)
            {
                m_point[x][y].rectFlag = true;
                m_point[active_X][active_Y].next[temp] = &m_point[x][y];
                m_point[x][y].previous[0] = &m_point[active_X][active_Y];
                m_point[x][y].color[0] = m_point[active_X][active_Y].color[0];
                m_point[x][y].color[1] = m_point[active_X][active_Y].color[0];
                active_X = x;
                active_Y = y;
                m_noMore = false;
            }
            else if(m_point[x][y].origin != true &&
               m_point[x][y].color[0] != m_point[active_X][active_Y].color[0] && !m_noMore)
            {
                m_point[x][y].rectFlag = true;
                m_point[active_X][active_Y].next[temp] = &m_point[x][y];
                m_point[x][y].getCovered();
                m_point[x][y].previous[1] = &m_point[active_X][active_Y];
                m_point[x][y].color[0] = m_point[active_X][active_Y].color[0];
                active_X = x;
                active_Y = y;
                m_noMore = false;
            }
            else if(m_point[active_X][active_Y].previous[1] && !m_point[x][y].origin)
            {
                if(m_point[x][y].color[0] == m_point[active_X][active_Y].color[0] &&
                                    m_point[x][y].x == m_point[active_X][active_Y].previous[1]->x &&
                                    m_point[x][y].y == m_point[active_X][active_Y].previous[1]->y)
                {
                    m_point[x][y].rectFlag = true;
                    m_point[active_X][active_Y].next[temp] = 0;
                    m_point[active_X][active_Y].previous[temp] = 0;
                    if(!m_point[active_X][active_Y].previous[0]->covered)
                        m_point[active_X][active_Y].getUncovered();
                    m_point[active_X][active_Y].color[0] = m_point[active_X][active_Y].color[1];
                    active_X = x;
                    active_Y = y;
                    temp = m_point[active_X][active_Y].previous[1]?1:0;
                    m_point[active_X][active_Y].next[temp] = 0;
                    m_noMore = false;
                    return;
                }
            }
            else if(m_point[x][y].color[0] == m_point[active_X][active_Y].color[0] &&
                    m_point[x][y].x == m_point[active_X][active_Y].previous[0]->x &&
                    m_point[x][y].y == m_point[active_X][active_Y].previous[0]->y)
            {
                m_point[x][y].rectFlag = true;
                if(!m_point[active_X][active_Y].origin)
                    m_point[active_X][active_Y].clear();
                else
                {
                    m_point[active_X][active_Y].previous[0] = 0;
                    m_point[active_X][active_Y].rectFlag = false;
                }

                active_X = x;
                active_Y = y;
                temp = m_point[active_X][active_Y].previous[1]?1:0;
                m_point[active_X][active_Y].next[temp] = 0;
                m_noMore = false;
                return;
            }
            else if(m_point[x][y].color[0] == m_point[active_X][active_Y].color[0] &&
                    m_point[x][y].origin && m_point[x][y].next[0] == 0)
            {
                m_point[x][y].rectFlag = true;
                m_point[active_X][active_Y].next[temp] = &m_point[x][y];
                m_point[x][y].previous[0] = &m_point[active_X][active_Y];
                active_X = x;
                active_Y = y;
                m_noMore = true;
            }
        }
    }
    pipeCheck();
    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    if(active_X != NOTACTIVE)
        if(m_point[active_X][active_Y].origin && m_point[active_X][active_Y].previous[0])
        {
            m_break.play();
        }
    bool isBreak = false;
    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
        {
            if(m_point[i][j].previous[1])
            {
                if(m_point[i][j].previous[0] && m_point[i][j].previous[1] !=
                        m_point[i][j].previous[0])
                    m_point[i][j].previous[0]->next[0] = 0;
                clearPoint(m_point[i][j].next[0]);
                m_point[i][j].color[1] = m_point[i][j].color[0];
                m_point[i][j].next[0] = m_point[i][j].next[1];
                m_point[i][j].previous[0] = m_point[i][j].previous[1];
                isBreak = true;
            }
            if(m_point[i][j].color[0] == blank)
                m_point[i][j].color[0] = m_point[i][j].color[1];
        }
    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
        {
            m_point[i][j].previous[1] = 0;
            m_point[i][j].next[1] = 0;
            m_point[i][j].covered = false;
        }
    if(isBreak)
        m_break.play();
    active_X = NOTACTIVE;
    active_Y = NOTACTIVE;
    for(int i = 0; i < 9; ++i)
    {
        if(!m_rectFlag[i])
        {
            m_move++;
            emit moveChange(m_move);
        }
        m_rectFlag[i] = true;
    }
    isWin();
    update();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;

    if (numDegrees.y() > 0)
        nextLevel();
    else
        previousLevel();
}

void MainWindow::clearPoint(Point *tempPoint)
{
    while(tempPoint)
    {
        if(tempPoint->previous[1])
        {
            if(tempPoint->previous[0] != tempPoint->previous[1])
                tempPoint->previous[0] = 0;
            tempPoint = tempPoint->next[0];
        }
        else if(!tempPoint->origin)
        {
            tempPoint->previous[0] = 0;
            tempPoint->previous[1] = 0;
            int tempX = tempPoint->x, tempY =  tempPoint->y;
            tempPoint->color[0] = blank;
            tempPoint->color[1] = blank;
            tempPoint->rectFlag = false;
            tempPoint->covered = false;
            tempPoint = tempPoint->next[0];
            m_point[tempX][tempY].next[0] = 0;
            m_point[tempX][tempY].next[1] = 0;
        } else
        {
            tempPoint->previous[0] = 0;
            tempPoint->previous[1] = 0;
            tempPoint->rectFlag = false;
            break;
        }
    }
}

bool MainWindow::isWin()
{
    bool flag = true;
    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
            if(m_point[i][j].color[0] == blank || (!m_point[i][j].next[0] && !(int)m_point[i][j].previous[0]))
            {
                flag = false;
                break;
            }
    if(flag)
    {
        QMediaPlayer winSound;
        winSound.setMedia(QUrl::fromLocalFile("/Sound/win.mp3"));
        winSound.setVolume(120);
        winSound.play();

        QIcon winIcon(":/Icon/finishedIcon.png");
        QPixmap winPixmap(":/Icon/finishedIcon.png");
        setEnabled(false);
        QMessageBox Win(this);

        Win.setWindowIcon(winIcon);
        Win.setWindowTitle("Win");
        Win.setIconPixmap(winPixmap);
        Win.setText("Congratulation!");
        Win.setButtonText(1, "Yes!");
        Win.exec();

        setEnabled(true);
        m_finishedLevel[m_chosenLevel-1] = 1;
        setLevel(m_chosenLevel+1);
        m_move = 0;
        emit moveChange(m_move);
        m_win = true;
        writeInfo();
        return true;
    }
    return false;
}

void MainWindow::nextLevel()
{
    setLevel(m_chosenLevel+1);
}

void MainWindow::previousLevel()
{
    setLevel(m_chosenLevel-1);
}

void MainWindow::toStart()
{
    close();
    m_mode = false;
    emit(backToStart());
}

void MainWindow::ReStart()
{
    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
        {
            if(!m_point[i][j].origin)
                m_point[i][j].clear();
            else
            {
                m_point[i][j].next[0] = 0;
                m_point[i][j].next[1] = 0;
                m_point[i][j].previous[0] = 0;
                m_point[i][j].previous[1] = 0;
                m_point[i][j].rectFlag = false;
                m_point[i][j].covered = false;
            }
        }
    m_move = 0;
    emit moveChange(m_move);
    pipeCheck();
    timeOver = 0;
    update();
}

void MainWindow::pipeCheck()
{
    double temp = 0, temp1 = 0;
    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
        {
            if(m_point[i][j].color[0] != blank)
                temp++;
            if(m_point[i][j].origin)
                temp1++;
        }
    emit pipeChange((temp - temp1) / ((m_level + 5) * (m_level + 5) - temp1) * 100);
}

void MainWindow::autoSolve()
{
    ReStart();
    m_win = false;
    for(int i = 0; i < 10; ++i)
    {
        for(int j = 0; j < 50; ++j)
            for(int k = 0; k < 2; ++k)
                m_path[i][j][k] = 0;
        m_pathLength[i] = 0;
    }

    for(int i = 0; i < m_colorNum; i++)
    {
        m_path[i][0][0] = m_origins[i*2][0];
        m_path[i][0][1] = m_origins[i*2][1];
        m_pathLength[i] = 1;
    }
    findPath(0);
    if(!m_win)
        QMessageBox::warning(this, "Wrong", "There is no answer", QMessageBox::Ok);
}

void MainWindow::findPath(int num)
{
    if(num >= m_colorNum)
    {
        for(int i = 0; i < m_colorNum; ++i)
            for(int j = 0; j < m_pathLength[i]-1; ++j)
            {
                m_point[m_path[i][j][0]][m_path[i][j][1]].next[0] = &m_point[m_path[i][j+1][0]][m_path[i][j+1][1]];
                m_point[m_path[i][j+1][0]][m_path[i][j+1][1]].previous[0] = &m_point[m_path[i][j][0]][m_path[i][j][1]];
            }
        isWin();
        return;
    }
    int x = m_path[num][m_pathLength[num]-1][0], y = m_path[num][m_pathLength[num]-1][1];
    for(int i = 0; i < 4; ++i)
    {
        if(x + stepX[i] > -1 && y + stepY[i] > -1 && x + stepX[i] < m_level + 5 &&
                y + stepY[i] < m_level + 5)
        {
            if(!m_point[x + stepX[i]][y + stepY[i]].color[0])
            {
                m_point[x + stepX[i]][y + stepY[i]].color[0] = m_point[x][y].color[0];
                m_path[num][m_pathLength[num]][0] = x + stepX[i];
                m_path[num][m_pathLength[num]][1] = y + stepY[i];
                m_pathLength[num]++;
                findPath(num);
                if(m_win)
                    return;
                m_pathLength[num]--;
                m_point[x + stepX[i]][y + stepY[i]].color[0] = blank;
            }
            else if((x + stepX[i] == m_origins[num*2+1][0])
                    && (y + stepY[i] == m_origins[num*2+1][1]))
            {
                m_path[num][m_pathLength[num]][0] = x + stepX[i];
                m_path[num][m_pathLength[num]][1] = y + stepY[i];
                m_pathLength[num]++;
                findPath(num+1);
                if(m_win)
                    return;
                m_pathLength[num]--;
            }
        }
    }
    return;
}

void MainWindow::writeInfo()
{
    QFile file("info.ifo");
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream txtOutput(&file);

    for(int i = 0; i < m_totalLevel; ++i)
        txtOutput << m_finishedLevel[i] << " ";
    file.close();
}

void MainWindow::readInfo()
{
    QFile file("info.ifo");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream txtInput(&file);

    for(int i = 0; i < m_totalLevel; ++i)
        txtInput >> m_finishedLevel[i];
    file.close();
}

void MainWindow::bgmPlay()
{
    m_bgm.setMedia(QUrl::fromLocalFile("/Sound/bgm.mp3"));
    m_bgm.setVolume(50);
    m_bgm.play();
    QTimer::singleShot(245000, this, SLOT(bgmPlay()));
}

void MainWindow::help()
{
    QIcon helpIcon(":/Icon/helpIcon.png");
    QPixmap helpPixmap(":/Icon/helpIcon.png");
    setEnabled(false);
    QMessageBox help(this);

    help.setWindowIcon(helpIcon);
    help.setWindowTitle("Help");
    help.setIconPixmap(helpPixmap);
    help.setText("  Flow Free is a simple yet addictive puzzle game.\n"
                 "  Connect matching colors with pipe to create a flow. Pair all colors, and cove r "
                 "the entire board to solve each puzzle. But watch out, pipes will break if they "
                 "cross or overlap!\n"
                 "  Free play through lots of levels, or race against the clock in Time Trial mode. "
                 "Gameplay ranges from simple and relaxed, to challenging and frenetic, and everywhere "
                 "in between. How you play is up to you. So, give Flow Free a try, and experience \"mind like water\"!");
    help.setButtonText(1, "I know.");
    help.exec();

    setEnabled(true);
}

void MainWindow::youLose()
{
    QMessageBox::information(this, "Lose", "Oh, you fail.", QMessageBox::No);
    ReStart();
}

void MainWindow::timeIsOut()
{
    timeOver++;
    ui->lcdNumber_4->display(timeOver-1);
    if(m_mode)
    {
        if(timeOver > (m_chosenLevel / 30 + 1) * 10)
            youLose();
    }
}

QString setLevelNum(int num)
{
    QString str = ":/Source/Level/level" + QString::number(num) + ".txt";
    return str;
}

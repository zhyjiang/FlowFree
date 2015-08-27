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

namespace Resource{
QColor color[9] = {QColor(237, 28, 36), QColor(0, 162, 232), QColor(102, 24, 126),
                   QColor(244, 233, 11), QColor(255, 127, 39), QColor(144, 233, 50),
                   QColor(0, 0, 0), QColor(185, 122, 87), QColor(254, 109, 221)};
QColor activeColor[9] = {QColor(237, 28, 36, 100), QColor(0, 162, 232, 100),
                         QColor(102, 24, 126, 100), QColor(244, 233, 11, 100),
                         QColor(255, 127, 39, 100), QColor(144, 233, 50, 100),
                         QColor(0, 0, 0, 100), QColor(185, 122, 87, 100),
                         QColor(254, 109, 221, 100)};
int stepX[4] = {-1, 0, 0, 1};
int stepY[4] = {0, -1, 1, 0};
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
    m_break(":/Sound/break.wav", this),
    m_bgm("SovietMarch.wav", this)
{
    m_start = false;
    ui->setupUi(this);
    m_chosenLevel = 1;
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    m_bgm.play();

    connect(ui->Next_Level, SIGNAL(triggered()), this, SLOT(nextLevel()));
    connect(ui->Previous_Level, SIGNAL(triggered()), this, SLOT(previousLevel()));
    connect(ui->Back_to_Start, SIGNAL(triggered()), this, SLOT(toStart()));
    connect(ui->ReStart, SIGNAL(triggered()), this, SLOT(ReStart()));
    connect(ui->next, SIGNAL(clicked()), this, SLOT(nextLevel()));
    connect(ui->back, SIGNAL(clicked()), this, SLOT(previousLevel()));
    connect(ui->reStart, SIGNAL(clicked()), this, SLOT(ReStart()));
    connect(ui->actionAutoSolve, SIGNAL(triggered()), this, SLOT(autoSolve()));
    connect(ui->autoSolve, SIGNAL(clicked(bool)), this, SLOT(autoSolve()));
    connect(this, SIGNAL(levelChange(int)), ui->lcdNumber_3, SLOT(display(int)));
    connect(this, SIGNAL(moveChange(int)), ui->lcdNumber, SLOT(display(int)));
    connect(this, SIGNAL(pipeChange(double)), ui->lcdNumber_2, SLOT(display(double)));
    QTimer::singleShot(165000, this, SLOT(bgmPlay()));

    QFile file(setLevelNum(m_chosenLevel));
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream txtInput(&file);
    txtInput >> m_level >> m_colorNum;

    for(int i = 0; i < m_colorNum * 2; ++i)
    {
        txtInput >> m_origins[i][0] >> m_origins[i][1];
        m_point[m_origins[i][0]][m_origins[i][1]].color = i/2+1;
        m_point[m_origins[i][0]][m_origins[i][1]].origin = true;
    }

    active_X = NOTACTIVE;
    active_Y = NOTACTIVE;
    m_move = 0;
    m_noMore = false;
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

void MainWindow::start()
{
    setEnabled(true);
    show();
    pipeCheck();
    m_start = true;
}

void MainWindow::leave()
{
    close();
}

void MainWindow::setLevel(int level)
{
    m_chosenLevel = level;
    if(m_chosenLevel < 1)
    {
        m_chosenLevel = 1;
        QMessageBox::information(this, "Wrong", "This is the first level.", QMessageBox::Ok);
    }
    else if(m_chosenLevel == 91)
    {
        QMessageBox::information(this, "Congratulation", "Finish all the levels.", QMessageBox::Yes);
        close();
        emit backToStart();
        return;
    }

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
        m_point[m_origins[i][0]][m_origins[i][1]].color = i/2+1;
        m_point[m_origins[i][0]][m_origins[i][1]].origin = true;
    }
    active_X = NOTACTIVE;
    active_Y = NOTACTIVE;
    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
            m_point[i][j].setPosition(i, j);

    m_widht = 110 + 70 * (m_level + 5);
    m_height = 100 + 70 * (m_level + 5) + 20;
    resize(m_widht, m_height);

    move((QApplication::desktop()->width() - (50 + 70 * (m_level + 5)))/2,
         (QApplication::desktop()->height() - (50 + 70 * (m_level + 5) + 50))/2);

    m_move = 0;
    emit moveChange(m_move);
    emit levelChange(m_chosenLevel);

    pipeCheck();

    update();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    if(m_start)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        for (int i = 1; i < m_level + 5; ++i)
        {
            painter.drawLine(55, 45 + i * 70, m_widht - 55, 45 + i * 70);
            painter.drawLine(55 + i * 70, 45, 55 + i * 70, m_height - 75);
        }

        for (int i = 0; i < m_level + 5; ++i)
            for (int j = 0; j < m_level + 5; ++j)
            {
                if(m_point[i][j].color != blank && m_point[i][j].rectFlag && m_rectFlag[m_point[i][j].color])
                {
                    painter.setPen(activeColor[m_point[i][j].color-1]);
                    painter.setBrush(activeColor[m_point[i][j].color-1]);
                    painter.drawRect(55 + 70 * i, 45 + 70 * j, 70, 70);
                }
            }

        for (int i = 0; i < m_level + 5; ++i)
            for (int j = 0; j < m_level + 5; ++j)
            {
                if(m_point[i][j].color != blank && m_point[i][j].origin)
                {
                    painter.setPen(color[m_point[i][j].color-1]);
                    painter.setBrush(color[m_point[i][j].color-1]);
                    painter.drawEllipse(QPoint(90 + 70 * i, 80 + 70 * j), 20, 20);
                }
                if(m_point[i][j].previous != 0)
                {
                    painter.setPen(QPen(QBrush(color[m_point[i][j].color-1]),
                            15, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.setBrush(color[m_point[i][j].color-1]);
                    painter.drawLine(QPoint(90 + 70 * m_point[i][j].previous->x, 80 + 70 * m_point[i][j].previous->y),
                                     QPoint(90 + 70 * i, 80 + 70 * j));
                }
            }

        if(active_X != NOTACTIVE)
        {
            if(m_point[active_X][active_Y].color != blank)
            {
                painter.setPen(activeColor[m_point[active_X][active_Y].color-1]);
                painter.setBrush(activeColor[m_point[active_X][active_Y].color-1]);
                painter.drawEllipse(mousePosition, 40, 40);
            }
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *mouseEvent)
{
    if(mouseEvent->x() > 55 && mouseEvent->y() > 25 && mouseEvent->x() < m_widht - 55
            && mouseEvent->y() < m_height - 75)
    {
        int x = floor((mouseEvent->x()-55) / 70);
        int y = floor((mouseEvent->y()-45) / 70);
        active_X = x;
        active_Y = y;
        m_noMore = false;
        mousePosition.setX(mouseEvent->x());
        mousePosition.setY(mouseEvent->y());
        if(m_point[x][y].color != blank)
            m_rectFlag[m_point[x][y].color] = false;
        if(m_point[x][y].color != blank && m_point[x][y].next != 0)
        {
            Point *tempPoint = m_point[x][y].next;
            m_point[x][y].next = 0;
            m_point[x][y].rectFlag = true;
            clearPoint(tempPoint);
        }
        if(m_point[x][y].origin)
        {
            for (int i = 0; i < m_level + 5; ++i)
                for (int j = 0; j < m_level + 5; ++j)
                {
                    if(m_point[i][j].color == m_point[x][y].color && !m_point[i][j].origin)
                        m_point[i][j].clear();
                    if(m_point[i][j].origin && m_point[i][j].color == m_point[x][y].color)
                    {
                        m_point[i][j].next = 0;
                        m_point[i][j].previous = 0;
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
    if(mouseEvent->x() > 55 && mouseEvent->y() > 25 && mouseEvent->x() < m_widht - 55
            && mouseEvent->y() < m_height - 75)
    {
        int x = floor((mouseEvent->x()-55) / 70);
        int y = floor((mouseEvent->y()-45) / 70);

        mousePosition.setX(mouseEvent->x());
        mousePosition.setY(mouseEvent->y());
        if(mouseEvent->buttons() & Qt::LeftButton && active_X != NOTACTIVE &&
                abs(active_X - x) + abs(active_Y - y) == 1 && m_point[active_X][active_Y].next == 0 &&
                    m_point[active_X][active_Y].color != blank)
        {
            if(m_point[x][y].origin != true &&
              m_point[x][y].color != m_point[active_X][active_Y].color && !m_noMore)
            {
                m_point[x][y].rectFlag = true;
                if(m_point[x][y].color != blank)
                    m_break.play();
                m_point[active_X][active_Y].next = &m_point[x][y];
                if(m_point[x][y].previous)
                    m_point[x][y].previous->next = 0;
                m_point[x][y].previous = &m_point[active_X][active_Y];
                m_point[x][y].color = m_point[active_X][active_Y].color;
                active_X = x;
                active_Y = y;
                if(m_point[x][y].next != 0)
                {
                    Point *tempPoint = m_point[x][y].next;
                    m_point[x][y].next = 0;
                    clearPoint(tempPoint);
                }
                m_noMore = false;
            }
            else if(m_point[x][y].color == m_point[active_X][active_Y].color &&
                    m_point[x][y].x == m_point[active_X][active_Y].previous->x &&
                    m_point[x][y].y == m_point[active_X][active_Y].previous->y)
            {
                m_point[x][y].rectFlag = true;
                if(!m_point[active_X][active_Y].origin)
                    m_point[active_X][active_Y].clear();
                else
                {
                    m_point[active_X][active_Y].previous = 0;
                    m_point[active_X][active_Y].rectFlag = false;
                }
                active_X = x;
                active_Y = y;
                m_point[active_X][active_Y].next = 0;
                m_noMore = false;
                return;
            }
            else if(m_point[x][y].color == m_point[active_X][active_Y].color &&
                    m_point[x][y].origin == true && m_point[x][y].next == 0)
            {
                m_point[x][y].rectFlag = true;
                m_point[active_X][active_Y].next = &m_point[x][y];
                m_point[x][y].previous = &m_point[active_X][active_Y];
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
        if(m_point[active_X][active_Y].origin && m_point[active_X][active_Y].previous)
        {
            m_break.play();
        }
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

void MainWindow::clearPoint(Point *tempPoint)
{
    while(tempPoint)
    {
        if(!tempPoint->origin)
        {
            tempPoint->previous = 0;
            int tempX = tempPoint->x, tempY =  tempPoint->y;
            tempPoint->color = blank;
            tempPoint->rectFlag = false;
            tempPoint = tempPoint->next;
            m_point[tempX][tempY].next = 0;
        } else
        {
            tempPoint->previous = 0;
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
            if(m_point[i][j].color == blank || (!m_point[i][j].next && !(int)m_point[i][j].previous))
            {
                flag = false;
                break;
            }
    if(flag)
    {
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
                m_point[i][j].next = 0;
                m_point[i][j].previous = 0;
                m_point[i][j].rectFlag = false;
            }
        }
    m_move = 0;
    emit moveChange(m_move);
    pipeCheck();
    update();
}

void MainWindow::pipeCheck()
{
    double temp = 0, temp1 = 0;
    for (int i = 0; i < m_level + 5; ++i)
        for (int j = 0; j < m_level + 5; ++j)
        {
            if(m_point[i][j].color != blank)
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
    findPath(m_origins[0][0], m_origins[0][1], 0);
    if(!m_win)
        QMessageBox::warning(this, "Wrong", "There is no answer", QMessageBox::Ok);
}

void MainWindow::findPath(int x, int y, int num)
{
    bool flag = false;
    if(num >= m_colorNum*2)
    {
        isWin();
        return;
    }
    for(int i = 0; i < 4; ++i)
    {
        if(x + stepX[i] > -1 && y + stepY[i] > -1 &&
                x + stepX[i] < m_level + 5 && y + stepY[i] < m_level + 5)
        {
            if(m_point[x + stepX[i]][y + stepY[i]].color == blank)
            {
                m_point[x + stepX[i]][y + stepY[i]].color = m_point[x][y].color;
                m_point[x + stepX[i]][y + stepY[i]].previous = &m_point[x][y];
                m_point[x][y].next = &m_point[x + stepX[i]][y + stepY[i]];
                flag = true;
                findPath(x + stepX[i], y + stepY[i], num);
                if(m_win)
                    return;
            }
            else if(m_point[x + stepX[i]][y + stepY[i]].color == m_point[x][y].color &&
                    m_point[x + stepX[i]][y + stepY[i]].origin && !m_point[x + stepX[i]][y + stepY[i]].previous)
            {
                m_point[x + stepX[i]][y + stepY[i]].previous = &m_point[x][y];
                m_point[x][y].next = &m_point[x + stepX[i]][y + stepY[i]];
                findPath(m_origins[num+2][0], m_origins[num+2][1], num+2);
                if(m_win)
                    return;
                flag = true;
            }
            else
                continue;

            if(flag)
            {
                if(!m_point[x + stepX[i]][y + stepY[i]].origin)
                    m_point[x + stepX[i]][y + stepY[i]].color = blank;
                m_point[x + stepX[i]][y + stepY[i]].previous = 0;
                m_point[x][y].next = 0;
            }
        }
    }
}

void MainWindow::writeInfo()
{
    QFile file("info.ifo");
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream txtOutput(&file);

    for(int i = 0; i < m_totalLevel; ++i)
        txtOutput << m_finishedLevel[i] << " ";
}

void MainWindow::readInfo()
{
    QFile file("info.ifo");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream txtInput(&file);

    for(int i = 0; i < m_totalLevel; ++i)
        txtInput >> m_finishedLevel[i];
}

void MainWindow::bgmPlay()
{
    m_bgm.play();
}

QString setLevelNum(int num)
{
    QString str = ":/Source/Level/level" + QString::number(num) + ".txt";
    return str;
}

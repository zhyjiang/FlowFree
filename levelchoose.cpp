#include "levelchoose.h"
#include "global.h"
#include "ui_levelchoose.h"
#include <QMessageBox>
#include <QDesktopWidget>
#include <QWheelEvent>
#include <QDebug>

using Resource::color;

LevelChoose::LevelChoose(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LevelChoose),
    m_map(this), currentNum(1), currentWorld(1)
{
    ui->setupUi(this);
    setWindowTitle("Flow Free");
    QIcon icon(":/Icon/icon.png");
    QFont font("Buxton Sketch", 14);
    QPalette pal;

    for(int i = 0; i < 90; ++i)
    {
        QString name = "Level " + QString::number(i+1);
        QPushButton* temp = new QPushButton(name, this);
        temp->setFlat(true);
        temp->setVisible(false);
        temp->setIcon(icon);
        temp->setFont(font);
        connect(temp, SIGNAL(clicked()), &m_map, SLOT(map()));
        m_map.setMapping(temp, i+1);
        m_levelButton.push_back(temp);
        temp = 0;
    }

    connect(&m_map, SIGNAL(mapped(int)), this, SLOT(slotLevel(int)));
    connect(&m_map, SIGNAL(mapped(int)), this, SLOT(chooseOver()));

    for(int i = 0; i < 10; ++i)
    {
        pal.setColor(QPalette::ButtonText, QColor(237, 28, 36));
        m_levelButton[i]->setPalette(pal);
        ui->choosePlatform->addWidget(m_levelButton[i], i/2, i%2);
        m_levelButton[i]->setVisible(true);
    }

    font.setPointSize(30);
    ui->worldTitle->setAlignment(Qt::AlignCenter);
    pal.setColor(QPalette::WindowText, color[0]);
    ui->worldTitle->setPalette(pal);
    ui->worldTitle->setFont(font);
    ui->worldTitle->setText("World " + QString::number(currentWorld) + "-"
                            +  QString::number(currentNum) + "\n" + QString::number(currentWorld+4) +
                            "×" + QString::number(currentWorld+4));

    readInfo();

    move((QApplication::desktop()->width() - width())/2,(QApplication::desktop()->height() - height())/2);
}

LevelChoose::~LevelChoose()
{
    delete ui;
    m_levelButton.clear();
}

void LevelChoose::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;

    if (numDegrees.y() > 0)
        selectPage(true);
    else
        selectPage(false);
}

void LevelChoose::slotLevel(int level)
{
    emit emitLevel(level);
}

void LevelChoose::chooseOver()
{
    close();
    emit over();
}

void LevelChoose::start()
{
    readInfo();
    exec();
    move((QApplication::desktop()->width() - width())/2,(QApplication::desktop()->height() - height())/2);
}

void LevelChoose::on_Next_clicked()
{
    selectPage(true);
}

void LevelChoose::on_Back_clicked()
{
    selectPage(false);
}

void LevelChoose::selectPage(bool state)
{
    int temp = (currentWorld-1)*30 + (currentNum-1)*10;
    if(state)
    {
        if(currentNum == 3 && currentWorld == 3)
            return;
        currentNum++;
        if(currentNum == 4)
        {
            currentNum = 1;
            currentWorld++;
        }
    }
    else
    {
        if(currentNum == 1 && currentWorld == 1)
            return;
        currentNum--;
        if(currentNum == 0)
        {
            currentNum = 3;
            currentWorld--;
        }
    }

    QIcon icon(":/Icon/icon.png");
    QFont font("Buxton Sketch", 30);
    QPalette pal;

    for(int i = temp; i < temp+10; ++i)
    {
        m_levelButton[i]->hide();
    }

    temp = (currentWorld-1)*30 + (currentNum-1)*10;
    ui->worldTitle->setText("World " + QString::number(currentWorld) + "-"
                            +  QString::number(currentNum) + "\n" + QString::number(currentWorld+4) +
                            "×" + QString::number(currentWorld+4));
    ui->worldTitle->setAlignment(Qt::AlignCenter);
    pal.setColor(QPalette::WindowText, color[temp/10]);
    ui->worldTitle->setPalette(pal);
    ui->worldTitle->setFont(font);

    font.setPointSize(14);
    for(int i = temp; i < temp+10; ++i)
    {
        pal.setColor(QPalette::ButtonText, color[temp/10]);
        m_levelButton[i]->setPalette(pal);
        ui->choosePlatform->addWidget(m_levelButton[i], (i-temp)/2, (i-temp)%2);
        m_levelButton[i]->setVisible(true);
    }
}

void LevelChoose::readInfo()
{
    QFile file("info.ifo");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream txtInput(&file);
        int temp;
        for(int i = 0; i < 90; ++i)
        {
            txtInput >> temp;
            if(temp)
                m_levelButton[i]->setIcon(QIcon(":/Icon/finishedIcon.png"));
        }
    }
}

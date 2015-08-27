#include "dialog.h"
#include "ui_dialog.h"
#include <QPixmap>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    m_levelChoose(this)
{
    ui->setupUi(this);
    setWindowTitle("Flow Free");
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint |  Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->leave, SIGNAL(clicked()), this, SLOT(leaveReceivers()));
    connect(ui->choose, SIGNAL(clicked()), &m_levelChoose, SLOT(start()));
    connect(&m_levelChoose, SIGNAL(emitLevel(int)), this, SLOT(slotLevel(int)));
    connect(&m_levelChoose, SIGNAL(over()), this, SLOT(startReceivers()));
    connect(&m_levelChoose, SIGNAL(over()), this, SLOT(close()));

    QPixmap title(":/Icon/title.png");
    ui->title->setPixmap(title);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::startReceivers()
{
    emit start();
}

void Dialog::leaveReceivers()
{
    close();
    emit leave();
}

void Dialog::slotLevel(int level)
{
    emit emitLevel(level);
}

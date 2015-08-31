#include "dialog.h"
#include "ui_dialog.h"
#include <QPixmap>
#include <QMessageBox>
#include <QIcon>
#include <QFile>
#include <QTextStream>
#include <QDebug>

Dialog::Dialog(QApplication *a, QWidget *parent) :
    app(a),
    QDialog(parent),
    ui(new Ui::Dialog),
    m_levelChoose(this)
{
    ui->setupUi(this);
    setWindowTitle("Flow Free");
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint |  Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->leave, SIGNAL(clicked()), this, SLOT(leaveReceivers()));
    connect(ui->choose, SIGNAL(clicked()), &m_levelChoose, SLOT(normalMode()));
    connect(ui->timeMode, SIGNAL(clicked()), &m_levelChoose, SLOT(timeMode()));
    connect(&m_levelChoose, SIGNAL(emitLevel(int)), this, SLOT(slotLevel(int)));
    connect(&m_levelChoose, SIGNAL(over(bool)), this, SLOT(startReceivers(bool)));
    connect(&m_levelChoose, SIGNAL(over(bool)), this, SLOT(close()));
    connect(ui->help, SIGNAL(clicked(bool)), this, SLOT(help()));
    connect(ui->clearInfo, SIGNAL(clicked()), this, SLOT(clearInfo()));

    QPixmap title(":/Icon/title.png");
    ui->title->setPixmap(title);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::startReceivers(bool mode)
{
    emit start(mode);
}

void Dialog::leaveReceivers()
{
    qApp->quit();
}

void Dialog::slotLevel(int level)
{
    emit emitLevel(level);
}

void Dialog::help()
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
                 "  Free play through hundreds of levels, or race against the clock in Time Trial mode. "
                 "Gameplay ranges from simple and relaxed, to challenging and frenetic, and everywhere "
                 "in between. How you play is up to you. So, give Flow Free a try, and experience \"mind like water\"!");
    help.setButtonText(1, "I know.");
    help.exec();

    setEnabled(true);
}

void Dialog::clearInfo()
{
    QIcon warningIcon(":/Icon/warningIcon.png");
    QPixmap warningPixmap(":/Icon/warningIcon.png");
    QMessageBox help(this);
    help.setStandardButtons(QMessageBox::NoButton);
    help.setWindowIcon(warningIcon);
    help.setWindowTitle("Warning");
    help.setIconPixmap(warningPixmap);
    help.setText("Warning, do you want to clear all the level info?(The icon will be updated in a new game.)");
    help.addButton("Yes", QMessageBox::AcceptRole);
    help.addButton("No", QMessageBox::RejectRole);
    connect(&help, SIGNAL(rejected()), this, SLOT(clear()));
    help.exec();
}

void Dialog::clear()
{
    QFile file("info.ifo");
    int temp = 0;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream txtInput(&file);
        int temp1;
        while(!txtInput.atEnd())
        {
            txtInput >> temp1;
            temp++;
        }
        file.close();
        QFile file2("info.ifo");
        if(file2.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream txtOutput(&file2);
            temp--;
            for(int i = 0; i < temp; ++i)
            {
                qDebug() << temp;
                txtOutput << 0 << " ";
            }
        }
    }
}

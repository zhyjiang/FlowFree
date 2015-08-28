#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "levelchoose.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

public slots:
    void startReceivers();
    void leaveReceivers();
    void slotLevel(int);
    void help();

private slots:
    void clearInfo();
    void clear();

signals:
    void start();
    void leave();
    void emitLevel(int);
    void haveCleaned();

private:
    Ui::Dialog *ui;
    LevelChoose m_levelChoose;
};

#endif // DIALOG_H

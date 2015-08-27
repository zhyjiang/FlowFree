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

signals:
    void start();
    void leave();
    void emitLevel(int);

private:
    Ui::Dialog *ui;
    LevelChoose m_levelChoose;
};

#endif // DIALOG_H

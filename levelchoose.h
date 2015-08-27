#ifndef LEVELCHOOSE_H
#define LEVELCHOOSE_H

#include <QDialog>
#include <QVector>
#include <QPushButton>
#include <QSignalMapper>

namespace Ui {
class LevelChoose;
}

class LevelChoose : public QDialog
{
    Q_OBJECT

public:
    explicit LevelChoose(QWidget *parent = 0);
    ~LevelChoose();

public slots:
    void slotLevel(int);
    void chooseOver();
    void selectPage(bool);
    void start();

signals:
    void emitLevel(int);
    void over();

private slots:
    void on_Next_clicked();
    void on_Back_clicked();

private:
    Ui::LevelChoose *ui;
    QVector<QPushButton*> m_levelButton;
    QSignalMapper m_map;
    int currentWorld;
    int currentNum;
};

#endif // LEVELCHOOSE_H

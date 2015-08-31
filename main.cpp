#include "mainwindow.h"
#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    Dialog startMenu(&app);

    QObject::connect(&startMenu, SIGNAL(start(bool)), &w, SLOT(start(bool)));
    QObject::connect(&startMenu, SIGNAL(start(bool)), &w, SLOT(start(bool)));
    QObject::connect(&startMenu, SIGNAL(leave()), &w, SLOT(leave()));
    QObject::connect(&startMenu, SIGNAL(emitLevel(int)), &w, SLOT(setLevel(int)));
    QObject::connect(&w, SIGNAL(backToStart()), &startMenu, SLOT(open()));

    startMenu.show();

    return app.exec();
}

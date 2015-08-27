#-------------------------------------------------
#
# Project created by QtCreator 2015-08-24T19:35:24
#
#-------------------------------------------------

QT       += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = project
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    point.cpp \
    dialog.cpp \
    levelchoose.cpp

HEADERS  += mainwindow.h \
    point.h \
    dialog.h \
    levelchoose.h \
    global.h

FORMS    += mainwindow.ui \
    dialog.ui \
    levelchoose.ui

RESOURCES += \
    resource.qrc

RC_ICONS = icon.ico

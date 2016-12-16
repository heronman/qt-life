#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T11:52:38
#
#-------------------------------------------------

QT       += core gui widgets

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QTLife
TEMPLATE = app

HEADERS  = mainwindow.h \
    LifeWidget.h \
    life.h \
    borderlayout.h \
    flowlayout.h \
    ruler.h \
    LifeBase.h \
    mlife.h \
    bmatrix.h \
    lifecached.h

SOURCES = main.cpp\
        mainwindow.cpp \
    LifeWidget.cpp \
    life.cpp \
    borderlayout.cpp \
    flowlayout.cpp \
    ruler.cpp \
    mlife.cpp \
    bmatrix.cpp \
    lifecached.cpp

RESOURCES = \
    QTLife.qrc

FORMS +=

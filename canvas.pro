#-------------------------------------------------
#
# Project created by QtCreator 2013-05-31T23:29:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = canvas
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    wacomthread.cpp

HEADERS  += mainwindow.h \
    wacomthread.h

FORMS    += mainwindow.ui

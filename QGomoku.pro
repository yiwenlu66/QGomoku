#-------------------------------------------------
#
# Project created by QtCreator 2016-08-30T19:22:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QGomoku
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    dialpad.cpp \
    gomokuboardwidget.cpp

HEADERS  += dialog.h \
    dialpad.h \
    gomokuboardwidget.h

FORMS    += dialog.ui \
    dialpad.ui

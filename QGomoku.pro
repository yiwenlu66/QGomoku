#-------------------------------------------------
#
# Project created by QtCreator 2016-08-30T19:22:14
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QGomoku
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    dialpad.cpp \
    gomokuboardwidget.cpp \
    createserverdialog.cpp

HEADERS  += dialog.h \
    dialpad.h \
    gomokuboardwidget.h \
    createserverdialog.h

FORMS    += dialog.ui \
    dialpad.ui \
    createserverdialog.ui

#-------------------------------------------------
#
# Project created by QtCreator 2011-08-21T22:58:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wave_demo
TEMPLATE = app

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    ../jhmi/jview.cpp \
    ../jhmi/jdefs.cpp \
    ../jhmi/jstoragehandler.cpp \
    ../jhmi/jwave.cpp

HEADERS  += \
    ../jhmi/jview.h \
    ../jhmi/jitems.h \
    ../jhmi/jdefs.h \
    ../jhmi/jstorage.h \
    ../jhmi/jstorage_p.h \
    ../jhmi/jstoragehandler.h \
    ../jhmi/jcacheditems.h \
    ../jhmi/jwave.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    mainwindow.qrc

QMAKE_CXX += -fpermissive

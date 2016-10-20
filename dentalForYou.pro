#-------------------------------------------------
#
# Project created by QtCreator 2015-03-19T17:52:54
#
#-------------------------------------------------

QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dentalForYou
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    preview.cpp \
    teethformula.cpp \
    teethformuladialog.cpp \
    loginwindow.cpp

HEADERS  += mainwindow.h \
    preview.h \
    teethformula.h \
    teethformuladialog.h \
    loginwindow.h \
    crypter.h

RESOURCES += \
    resources.qrc

DISTFILES +=

QT += core
QT += widgets

TEMPLATE = app
CONFIG += c++11 console
CONFIG += qt
CONFIG -= app_bundle

LIBS *= -pthread

include(../../../g/g.pri)

DESTDIR = $${PWD}/../../bin

SOURCES += \
        captiveportal.cpp \
        main.cpp

HEADERS += \
    captiveportal.h \
    stdafx.h

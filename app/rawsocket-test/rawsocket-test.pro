QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

DESTDIR = $${PWD}/../../bin

include(../../../g/g.pri)

INCLUDEPATH += $${PWD}/../../../opensocket/src
INCLUDEPATH += $${PWD}/../../../opensocket/external

LIBS += -L../../../opensocket/lib -lopensocket -lssl -lcrypto -lpthread

SOURCES += \
        werawclient.cpp \
        main.cpp

HEADERS += \
        werawclient.h

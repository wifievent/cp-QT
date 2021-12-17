QT += core

TEMPLATE = app
CONFIG += c++17 console
CONFIG += qt

LIBS *= -pthread

INCLUDEPATH *= $${PWD}/../../src

DESTDIR = $${PWD}/../../bin

include(../../../g/g.pri)

SOURCES += \
        main.cpp \
        ../../src/socket.cpp \
        ../../src/udpclient.cpp \
        ../../src/udpsocket.cpp

HEADERS += \
        ../../src/socket.h \
        ../../src/udp.h \
        ../../src/udpclient.h \
        ../../src/udpsocket.h

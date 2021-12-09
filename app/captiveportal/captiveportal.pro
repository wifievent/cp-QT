QT += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TEMPLATE = app
CONFIG += qt

include(../../../g/g.pri)
include(../../cp.pri)

DESTDIR = $${PWD}/../../bin

SOURCES += \
    ../../src/socket.cpp \
    ../../src/udpclient.cpp \
    ../../src/udpsocket.cpp \
    ../../src/udpserver.cpp \
    ../../src/httprequest.cpp \
    ../../src/httpresponse.cpp \
    ../../src/tcpsocket.cpp \
    ../../src/tcpclient.cpp \
    captiveportal.cpp \
    main.cpp \
    weudpclient.cpp \
    weudpserver.cpp \
    weupdater.cpp

HEADERS += \
    ../../src/socket.h \
    ../../src/udpclient.h \
    ../../src/udpsocket.h \
    ../../src/udpserver.h \
    ../../src/httprequest.h \
    ../../src/httpresponse.h \
    ../../src/tcpsocket.h \
    ../../src/tcpclient.h \
    ../../src/http.h \
    captiveportal.h \
    stdafx.h \
    weudpclient.h \
    weudpserver.h \
    weupdater.h

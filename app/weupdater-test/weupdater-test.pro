QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

include(../../cp.pri)
include(../../../g/g.pri)

SOURCES += \
        main.cpp \
        weupdater.cpp \
        ../../src/httprequest.cpp \
        ../../src/httpresponse.cpp \
        ../../src/socket.cpp \
        ../../src/tcpsocket.cpp \
        ../../src/tcpclient.cpp

HEADERS += \
        stdafx.h \
        weupdater.h \
        ../../src/httprequest.h \
        ../../src/httpresponse.h \
        ../../src/socket.h \
        ../../src/tcpsocket.h \
        ../../src/tcpclient.h \
        ../../src/http.h \
        ../../src/tcp.h

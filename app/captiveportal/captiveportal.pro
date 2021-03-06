QT -= gui

CONFIG += c++2a
TEMPLATE = app
CONFIG += qt

include(../../../g/g.pri)
include(../../cp.pri)

DESTDIR = $${PWD}/../../bin

LIBS += -L../../../opensocket/lib -lopensocket -lssl -lcrypto -lpthread -lnetfilter_queue -lglog

SOURCES += \
    ../../src/httprequest.cpp \
    ../../src/httpresponse.cpp \
    ../../../opensocket/src/*.cpp \
    ../../../opensocket/src/base/*.cpp \
    weuiserver.cpp \
    werawclient.cpp \
    captiveportal.cpp \
    main.cpp \
    wesslserver.cpp \
    weudpclient.cpp \
    weudpserver.cpp \
    weupdater.cpp

HEADERS += \
    ../../src/httprequest.h \
    ../../src/httpresponse.h \
    ../../src/http.h \
    ../../../opensocket/src/*.h \
    ../../../opensocket/src/base/*.h \
    weuiserver.h \
    werawclient.h \
    captiveportal.h \
    stdafx.h \
    wesslserver.h \
    weudpclient.h \
    weudpserver.h \
    weupdater.h

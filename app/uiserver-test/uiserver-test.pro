QT -= gui

CONFIG += c++2a
TEMPLATE = app
CONFIG += qt

include(../../cp.pri)

LIBS += -L../../../opensocket/lib -lopensocket -lssl -lcrypto -lpthread -lglog

SOURCES += \
        main.cpp \
        uiserver.cpp \
        ../../src/httprequest.cpp \
        ../../src/httpresponse.cpp \
        ../../../opensocket/src/*.cpp \
        ../../../opensocket/src/base/*.cpp

HEADERS += \
    uiserver.h \
    ../../src/httprequest.h \
    ../../src/httpresponse.h \
    ../../src/http.h \
    ../../../opensocket/src/*.h \
    ../../../opensocket/src/base/*.h

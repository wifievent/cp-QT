QT -= gui

CONFIG += c++17
TEMPLATE = app
CONFIG += qt

include(../../cp.pri)

LIBS += -L../../../opensocket/lib -lopensocket -lssl -lcrypto -lpthread

SOURCES += \
    main.cpp

HEADERS += \

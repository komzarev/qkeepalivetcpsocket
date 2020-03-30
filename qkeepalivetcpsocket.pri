QT -= gui
QT += network

CONFIG += c++11

INCLUDEPATH += $$PWD

HEADERS += $$PWD/keepalivetcpsocket.hpp

SOURCES += $$PWD/keepalivetcpsocket.cpp

windows {
    LIBS += -lws2_32
}
qnx {
    LIBS += -lsocket
}

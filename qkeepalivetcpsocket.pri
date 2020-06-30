QT -= gui
QT += network

CONFIG += c++11

INCLUDEPATH += $$PWD

HEADERS += $$PWD/keepalivetcpsocket.hpp \
    $$PWD/keepalivetcpoption.hpp

SOURCES += $$PWD/keepalivetcpsocket.cpp \
    $$PWD/keepalivetcpoption.cpp

windows {
    LIBS += -lws2_32
}
qnx {
    LIBS += -lsocket
}

TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp


QMAKE_CXXFLAGS += -std=c++0x

HEADERS += \
    factornode.h \
    addnode.h \
    equalitynode.h \
    evidencenode.h \
    factorgraph.h


TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    network.cpp \
    variable.cpp \
    examples.cpp

HEADERS += \
    variable.h \
    gaussian.h \
    network.h \
    gamma.h \
    mathutil.h \
    dirichlet.h \
    discrete.h \
    mixture.h \
    util.h \
    examples.h

OTHER_FILES += \
    TODO.md

QMAKE_CXXFLAGS += -std=c++11


# TODO: matlab support
LIBS += -lgsl -lblas




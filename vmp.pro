TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    network.cpp \
    variable.cpp \
    examples.cpp \
    algonquin.cpp \
    mathutil.cpp \
    matlab/persistentobject.cpp
#    matlab/mexfactorgraph.cpp

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
    examples.h \
    algonquin.h \
    matlab/persistentobject.h \
    matlab/convert.h

OTHER_FILES += \
    TODO.md \
    speechbin.txt \
    noisybin.txt

QMAKE_CXXFLAGS += -std=c++11

# TODO: matlab support
# LIBS += -lgsl -lblas
LIBS += -lprofiler




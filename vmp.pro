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
    gaussian.cpp \
    matlab/persistentobject.cpp \
    gaussianarray.cpp \
    discretearray.cpp
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
    matlab/convert.h \
    gaussianarray.h \
    discretearray.h \
    gammaarray.h

OTHER_FILES += \
    TODO.md \
    speechbin.txt \
    noisybin.txt

QMAKE_CXXFLAGS += -std=c++11 -Wno-unused-variable -Wno-unused-parameter

OBJECTS_DIR = ./.obj

# TODO: matlab support
# LIBS += -lgsl -lblas
LIBS += -lprofiler




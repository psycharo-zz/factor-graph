TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    variable.cpp \
    examples.cpp \
    mathutil.cpp \
    matlab/persistentobject.cpp \
    algonquin/mvalgonquin.cpp \
    algonquin/algonquin.cpp
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
    matlab/persistentobject.h \
    matlab/convert.h \
    gaussianarray.h \
    discretearray.h \
    gammaarray.h \
    wishart.h \
    mvgaussian.h \
    algonquin/algonquin.h \
    algonquin/algonquinnetwork.h \
    algonquin/mvalgonquin.h

INCLUDEPATH += alqonquin matlab ./libs/include

OTHER_FILES += \
    TODO.md \
    speechbin.txt \
    noisybin.txt

QMAKE_CXXFLAGS += -std=c++11 -Wno-unused-variable -Wno-unused-parameter

OBJECTS_DIR = ./.obj

# TODO: matlab support
# LIBS += -lgsl -lblas



LIBS += -L./libs/lib -lprofiler -larmadillo -L/opt/intel/lib/intel64/ /opt/intel/mkl/tools/builder/mkl_custom.so -liomp5 -lgsl -lgslcblas




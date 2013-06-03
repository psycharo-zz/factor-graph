TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    factor.cpp \
    sumfactor.cpp \
    gaussianmixture.cpp \
    network.cpp

HEADERS += \
    variable.h \
    gaussian.h \
    factor.h \
    sumfactor.h \
    gaussianmixture.h \
    network.h \
    gamma.h \
    examples.h

OTHER_FILES += \
    TODO.md




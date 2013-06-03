TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    network.cpp \
    variable.cpp

HEADERS += \
    variable.h \
    gaussian.h \
    network.h \
    gamma.h \
    mathutil.h

OTHER_FILES += \
    TODO.md


# TODO: matlab support
LIBS += -lgsl -lblas




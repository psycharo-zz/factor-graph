TEMPLATE = app

CONFIG -= qt
CONFIG += console


SOURCES += main.cpp \
    factornode.cpp \
    message.cpp \
    equalitynode.cpp \
    addnode.cpp \
    matlab/customnode.cpp \
    matlab/mexfactorgraph.cpp \
    network.cpp


QMAKE_CXXFLAGS += -std=c++0x


HEADERS += \
    factornode.h \
    addnode.h \
    equalitynode.h \
    evidencenode.h \
    factorgraph.h \
    message.h \
    util.h \
    multiplicationnode.h \
    tests/evidencenode_test.h \
    tests/util_test.h \
    tests/addnode_test.h \
    tests/equalitynode_test.h \
    tests/kalmanfilter_test.h \
    matlab/customnode.h \
    network.h \
    matlab/convert.h


# DEBUG
INCLUDEPATH += /home/timur/bin/matlab-2012b/extern/include

unix {
    LIBS += -llapack -lblas -L/usr/src/gtest -lgtest -lpthread
}


# TODO: windows
windows {
    INCLUDEPATH += ./libs/gtest-1.6.0/include ./libs/clapack-3.2.1-CMAKE/INCLUDE
    LIBS += -L./libs/gtest-1.6.0 -lgtest.lib
}








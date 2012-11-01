TEMPLATE = app

CONFIG -= qt
CONFIG += console


SOURCES += src/main.cpp \
    src/factornode.cpp \
    src/message.cpp \
    src/equalitynode.cpp \
    src/addnode.cpp \
    src/network.cpp \
    matlab/customnode.cpp \
    matlab/mexfactorgraph.cpp

OBJECTS_DIR = .obj

QMAKE_CXXFLAGS += -std=c++0x


HEADERS += \
    src/factornode.h \
    src/addnode.h \
    src/equalitynode.h \
    src/evidencenode.h \
    src/factorgraph.h \
    src/message.h \
    src/util.h \
    src/multiplicationnode.h \
    src/network.h \
    tests/evidencenode_test.h \
    tests/util_test.h \
    tests/addnode_test.h \
    tests/equalitynode_test.h \
    tests/kalmanfilter_test.h \
    tests/schedule_test.h \
    tests/multiplicationnode_test.h \
    matlab/convert.h \
    matlab/customnode.h \



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








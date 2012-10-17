TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    factornode.cpp \
    message.cpp \
    equalitynode.cpp \
    addnode.cpp


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
    tests/kalmanfilter_test.h


unix {
    LIBS += -llapack -lgsl -lgslcblas -L/usr/src/gtest -lgtest -lpthread
}

# TODO: windows






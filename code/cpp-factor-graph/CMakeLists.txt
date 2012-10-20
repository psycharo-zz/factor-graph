project(cpp-factor-graph)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS -std=gnu++0x)


list(APPEND SRC_LIST
main.cpp
util.h
addnode.cpp addnode.h
equalitynode.cpp equalitynode.h
factornode.cpp factornode.h
message.cpp message.h
multiplicationnode.h
tests/addnode_test.h
tests/equalitynode_test.h
tests/evidencenode_test.h
tests/kalmanfilter_test.h
tests/util_test.h
)



# for windows only
# TODO: find_library instead?
link_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/gtest-1.6.0/include")


list(APPEND LIBS_LIST
blas lapack
gtest pthread
)



add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries (${PROJECT_NAME}  ${LIBS_LIST})


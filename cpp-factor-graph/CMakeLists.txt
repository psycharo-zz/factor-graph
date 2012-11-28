project(cpp-factor-graph)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS -std=gnu++0x)

list(APPEND SRC_LIST
tests/main.cpp
matlab/ffg/src/customnode.cpp
matlab/ffg/src/mexfactorgraph.cpp
matlab/ffg/src/customnode.h
matlab/ffg/src/convert.h
tests/*.h

)



# for windows only
# TODO: find_library instead?
link_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/gtest-1.6.0/include" "src" ".")


list(APPEND LIBS_LIST
blas lapack
gtest pthread
)



add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries (${PROJECT_NAME}  ${LIBS_LIST})


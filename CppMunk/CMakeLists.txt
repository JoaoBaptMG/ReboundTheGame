cmake_minimum_required(VERSION 2.8.11)

project(CppMunk)

file(GLOB SRCS "src/*.cpp" "include/cppmunk/*.h")
set(INCS "${PROJECT_SOURCE_DIR}/include")
set(CPPMUNK_INCLUDE_DIR ${INCS} PARENT_SCOPE)

find_package(Chipmunk REQUIRED)

include_directories(${INCS}/cppmunk ${CHIPMUNK_INCLUDE_DIR})

add_library(CppMunk STATIC ${SRCS})


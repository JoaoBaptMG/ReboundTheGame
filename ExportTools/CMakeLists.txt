cmake_minimum_required(VERSION 2.8.11)

project(ExportTools)

file(GLOB SRCS "*.h" "*.hpp" "*.c" "*.cpp")

add_executable(ExportTools ${SRCS})

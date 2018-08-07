cmake_minimum_required(VERSION 2.8.11)

project(ExportTools)

file(GLOB SRCS "*.c" "*.cpp" "*.h" "*.hpp")

add_executable(ExportTools ${SRCS})
target_link_libraries(ExportTools Commons)

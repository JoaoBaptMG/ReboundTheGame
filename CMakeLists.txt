cmake_minimum_required(VERSION 2.8.11)

project(ReboundTheGame)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(SFML_STATIC_LIBRARIES ON)

add_subdirectory(CppMunk)
add_subdirectory(ExportTools)
add_subdirectory(MainGame)
add_subdirectory(Resources)

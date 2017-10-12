cmake_minimum_required(VERSION 2.8.11)

project(ReboundTheGame)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(SFML_STATIC_LIBRARIES ON)

set(COMMONS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Commons)

if(MSVC)
	add_definitions(-D_USE_MATH_DEFINES -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS /wd4244)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_definitions(/SAFESEH:no)
	endif()
endif()

add_subdirectory(CppMunk)
add_subdirectory(Coroutine)
add_subdirectory(ExportTools)
add_subdirectory(MainGame)
add_subdirectory(Resources)

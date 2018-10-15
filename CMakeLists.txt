cmake_minimum_required(VERSION 3.9)

project(ReboundTheGame)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(SFML_STATIC_LIBRARIES ON)

if(MSVC)
	add_definitions(-D_USE_MATH_DEFINES -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS /wd4244)
endif()

add_subdirectory(Commons)
add_subdirectory(CppMunk)
add_subdirectory(ExportTools)
add_subdirectory(MainGame)
add_subdirectory(Languages)
add_subdirectory(Resources)

add_custom_target(install_everything
	"${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install
	DEPENDS MainGame Languages Resources
	COMMENT "Installing ${PROJECT_NAME}")
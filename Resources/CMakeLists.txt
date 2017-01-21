cmake_minimum_required(VERSION 2.8.11)

project(Resources)

file(GLOB_RECURSE RESOURCES RELATIVE ${PROJECT_SOURCE_DIR} "*")
list(REMOVE_ITEM RESOURCES "CMakeLists.txt")

foreach(fname ${RESOURCES})
	add_custom_command(OUTPUT ${fname}
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/${fname} ${PROJECT_BINARY_DIR}/${fname}
                       MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/${fname})
endforeach()

add_custom_target(Resources ALL DEPENDS ${RESOURCES} SOURCES ${RESOURCES})

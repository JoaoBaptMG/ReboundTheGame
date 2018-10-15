cmake_minimum_required(VERSION 2.8.11)

project(Commons)

file(GLOB_RECURSE SRCS "*.cpp" "*.hpp")

# taken from https://stackoverflow.com/a/31987079
foreach(FILE ${SRCS})
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
	 
    source_group("${GROUP}" FILES "${FILE}")
endforeach()

set(INCS "${PROJECT_SOURCE_DIR}")
set(COMMONS_INCLUDE_DIR ${INCS} PARENT_SCOPE)

find_package(GLM REQUIRED)

include_directories(${INCS} ${GLM_INCLUDE_DIR})

add_library(Commons STATIC ${SRCS})


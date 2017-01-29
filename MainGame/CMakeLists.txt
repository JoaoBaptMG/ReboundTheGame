cmake_minimum_required(VERSION 2.8.11)

project(MainGame)

find_package(SFML COMPONENTS graphics window system REQUIRED)
find_package(Chipmunk REQUIRED)

include_directories(${SFML_INCLUDE_DIR} ${CHIPMUNK_INCLUDE_DIR} ${CPPMUNK_INCLUDE_DIR} ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE MainGame_SRCS "*.c" "*.cpp" "*.h" "*.hpp")

add_executable(MainGame ${MainGame_SRCS})
target_link_libraries(MainGame CppMunk ${CHIPMUNK_LIBRARIES} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

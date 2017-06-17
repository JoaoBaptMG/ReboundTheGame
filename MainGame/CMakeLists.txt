cmake_minimum_required(VERSION 2.8.11)

project(MainGame)

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

find_package(SFML COMPONENTS graphics window system REQUIRED)
find_package(Chipmunk REQUIRED)

include_directories(${COMMONS_INCLUDE_DIR} ${SFML_INCLUDE_DIR} ${CHIPMUNK_INCLUDE_DIR} ${CPPMUNK_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE MainGame_SRCS "*.c" "*.cpp" "*.h" "*.hpp")

add_executable(MainGame ${MainGame_SRCS})
target_link_libraries(MainGame CppMunk ${CHIPMUNK_LIBRARIES} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

install(TARGETS MainGame RUNTIME DESTINATION bin)

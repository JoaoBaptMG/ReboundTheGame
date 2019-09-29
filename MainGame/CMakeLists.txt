cmake_minimum_required(VERSION 3.12)

project(MainGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /SAFESEH:NO")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS")
	add_definitions(/ZI)
endif()

set(Boost_USE_STATIC_LIBS ON)

if (MSVC)
	find_package(SFML COMPONENTS graphics window system main REQUIRED)
else()
	find_package(SFML COMPONENTS graphics window system REQUIRED)
endif()

find_package(Chipmunk REQUIRED)
find_package(HarfBuzz REQUIRED)
find_package(Boost COMPONENTS context REQUIRED)
find_package(PortAudio REQUIRED)

include_directories(${COMMONS_INCLUDE_DIR} ${SFML_INCLUDE_DIR} ${CHIPMUNK_INCLUDE_DIR} ${Boost_INCLUDE_DIR}
    ${CPPMUNK_INCLUDE_DIR} ${HARFBUZZ_INCLUDE_DIR} ${PORTAUDIO_INCLUDE_DIR} ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE MainGame_SRCS "*.c" "*.cpp" "*.h" "*.hpp")

if(APPLE)
    file(GLOB_RECURSE MainGame_MMs "*.mm")
    set(MainGame_SRCS ${MainGame_SRCS} ${MainGame_MMs})
endif()

# taken from https://stackoverflow.com/a/31987079
foreach(FILE ${MainGame_SRCS})
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

add_executable(MainGame ${MainGame_SRCS})
target_link_libraries(MainGame CppMunk ${Boost_LIBRARIES} ${CHIPMUNK_LIBRARIES} ${SFML_LIBRARIES}
    ${HARFBUZZ_LIBRARY} ${PORTAUDIO_LIBRARIES} ${SFML_DEPENDENCIES})

install(TARGETS MainGame RUNTIME DESTINATION bin)


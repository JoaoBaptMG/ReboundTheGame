cmake_minimum_required(VERSION 2.8.11)

# taken from http://blog.audio-tk.com/2015/09/01/sorting-source-files-and-projects-in-folders-with-cmake-and-visual-studioxcode/
macro(source_group_by_folder target)
    set(SOURCE_GROUP_DELIMITER "/")
    set(last_dir "")
    set(files "")
    foreach(file ${${target}_SRCS})
        file(RELATIVE_PATH relative_file "${PROJECT_SOURCE_DIR}/${target}" ${file})
        get_filename_component(dir "${relaive_file}" PATH)
        if(NOT "${dir}" STREQUAL "${last_dir}")
            if(files)
                source_group("${last_dir}" FILES ${files})
            endif()
        endif()
        set(files ${files} ${file})
        set(last_dir "${dir}")
    endforeach()
    if(files)
        source_group("${last_dir}" FILES ${files})
    endif()
endmacro()

project(MainGame)

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
elseif(APPLE)
    set(MACOSX_BUNDLE TRUE)
endif()

find_package(SFML COMPONENTS graphics window system REQUIRED)
find_package(Chipmunk REQUIRED)
find_package(HarfBuzz REQUIRED)

include_directories(${COMMONS_INCLUDE_DIR} ${SFML_INCLUDE_DIR} ${CHIPMUNK_INCLUDE_DIR} ${CPPMUNK_INCLUDE_DIR}
    ${HARFBUZZ_INCLUDE_DIR} ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE MainGame_SRCS "*.c" "*.cpp" "*.h" "*.hpp")
source_group_by_folder(MainGame)

add_executable(MainGame ${MainGame_SRCS})
target_link_libraries(MainGame CppMunk ${CHIPMUNK_LIBRARIES} ${SFML_LIBRARIES} ${HARFBUZZ_LIBRARY} ${SFML_DEPENDENCIES})

install(TARGETS MainGame RUNTIME DESTINATION bin)


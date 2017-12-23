cmake_minimum_required(VERSION 2.8.11)

project(Resources)

file(GLOB LANGUAGES RELATIVE ${PROJECT_SOURCE_DIR} "*.lnx")

set(OUTPUTS "")

function(add_language fname)
    get_filename_component(fname_noext ${fname} NAME_WE)
    set(out_fname "${fname_noext}.lang")

    add_custom_command(OUTPUT ${out_fname}
                       COMMAND ExportTools exportLanguage ${PROJECT_SOURCE_DIR}/${fname} ${PROJECT_BINARY_DIR}/${out_fname}
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                       MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/${fname} DEPENDS ExportTools)
    set(OUTPUTS ${OUTPUTS} ${PROJECT_BINARY_DIR}/${out_fname} PARENT_SCOPE)
endfunction()

foreach(fname ${LANGUAGES})
    add_language(${fname})
endforeach()

add_custom_target(Languages ALL DEPENDS ${LANGUAGES} SOURCES ${LANGUAGES})
install(FILES ${OUTPUTS} DESTINATION bin/Languages)

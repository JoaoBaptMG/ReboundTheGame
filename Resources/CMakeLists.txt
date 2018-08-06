cmake_minimum_required(VERSION 2.8.11)

project(Resources)

file(GLOB_RECURSE RESOURCES RELATIVE ${PROJECT_SOURCE_DIR} "*")
list(REMOVE_ITEM RESOURCES "CMakeLists.txt")

set(OUTPUTS "")

function(add_resource fname)
    set(EXTENSIONS ".tmx" ".lvx" ".tsx" ".pex")
    set(TOOLS tmxToMap lvxToLvl tsxToTs pexToPe)
    set(TOOL_OUTPUTS ".map" ".lvl" ".ts" ".pe")
	set(COPY_EXTENSIONS ".png" ".ttf" ".wav")
	
    get_filename_component(ext ${fname} EXT)
    list(FIND EXTENSIONS ${ext} toolIndex)

    if(toolIndex EQUAL -1)
		list(FIND COPY_EXTENSIONS ${ext} toolIndex)
		if (NOT toolIndex EQUAL -1)
			add_custom_command(OUTPUT ${fname}
								COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/${fname} ${PROJECT_BINARY_DIR}/${fname}
								MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/${fname})
			set(OUTPUTS ${OUTPUTS} ${PROJECT_BINARY_DIR}/${fname} PARENT_SCOPE)
		endif()
    else()
        list(GET TOOLS ${toolIndex} tool)
        list(GET TOOL_OUTPUTS ${toolIndex} output)

        get_filename_component(fname_noext ${fname} NAME_WE)
        set(out_fname "${fname_noext}${output}")

        add_custom_command(OUTPUT ${out_fname}
                           COMMAND ExportTools ${tool} ${PROJECT_SOURCE_DIR}/${fname} ${PROJECT_BINARY_DIR}/${out_fname}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                           MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/${fname} DEPENDS ExportTools)
        set(OUTPUTS ${OUTPUTS} ${PROJECT_BINARY_DIR}/${out_fname} PARENT_SCOPE)
    endif()
endfunction()

foreach(fname ${RESOURCES})
    add_resource(${fname})
endforeach()

add_custom_target(Resources ALL DEPENDS ${RESOURCES} SOURCES ${RESOURCES})
install(FILES ${OUTPUTS} DESTINATION bin/Resources)

cmake_minimum_required(VERSION 2.8.11)

project(Coroutine C CXX)

file(GLOB SRCS "*.c" "*.cpp" "*.h" "*.hpp")

include("./TargetArch.cmake")
target_architecture(ARCH)

if (ARCH STREQUAL "i386" OR ARCH STREQUAL "ia64" OR ARCH STREQUAL "x86_64")
    enable_language(ASM)
    #if (NOT CMAKE_ASM_NASM_COMPILER_LOADED)
    #    message(FATAL_ERROR "NASM must be installed for this project build to ocurr on x86 platforms")
    #endif()
elseif (ARCH MATCHES "arm(v\d+)?")
    message(WARNING "ARM processors not supported (yet)")
endif()

if (WIN32)
    
elseif (UNIX)
     if (ARCH STREQUAL "i386")
        
     else()
        #set(CMAKE_ASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -F dwarf")
        list(APPEND SRCS "asm-impl/coroutine_unix64.s")
     endif()
elseif (APPLE)
    
else()
    message(FATAL_ERROR "Unknown platform for compiling Coroutine!")
endif()

add_executable(Coroutine ${SRCS})

cmake_minimum_required(VERSION 2.8.11)

project(Coroutine C CXX)

file(GLOB SRCS "*.c" "*.cpp" "*.h" "*.hpp")

include("./TargetArch.cmake")
target_architecture(ARCH)

enable_language(ASM)

if (WIN32)
    
elseif (UNIX)
     if (ARCH STREQUAL "i386")
        list(APPEND SRCS "asm-impl/coroutine_unix32.s")
     else()
        #set(CMAKE_ASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -F dwarf")
        list(APPEND SRCS "asm-impl/coroutine_unix64.s")
     endif()
elseif (APPLE)
    
else()
    message(FATAL_ERROR "Unknown platform for compiling Coroutine!")
endif()

add_executable(Coroutine ${SRCS})

cmake_minimum_required(VERSION 2.8.11)

project(Coroutine C CXX)

file(GLOB SRCS "*.c" "*.cpp" "*.h" "*.hpp")
list(APPEND SRCS "asm-impl/coroutine.h")

include("./TargetArch.cmake")
target_architecture(ARCH)

if (NOT WIN32)
	enable_language(ASM)
endif()

if (WIN32)
	list(APPEND SRCS "asm-impl/coroutine-win-fiber.c")
elseif (UNIX)
	if (ARCH STREQUAL "i386")
        list(APPEND SRCS "asm-impl/coroutine-unix-i386.s")
    else()
        list(APPEND SRCS "asm-impl/coroutine-unix-x64.s")
    endif()
elseif (APPLE)
    
else()
    message(FATAL_ERROR "Unknown platform for compiling Coroutine!")
endif()

add_executable(Coroutine ${SRCS})

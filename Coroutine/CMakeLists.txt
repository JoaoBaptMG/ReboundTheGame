cmake_minimum_required(VERSION 2.8.11)

project(Coroutine C CXX)

file(GLOB_RECURSE SRCS "*.c" "*.cpp" "*.h" "*.hpp")

include("./TargetArch.cmake")
target_architecture(ARCH)

if (WIN32)
	enable_language(ASM_MASM)
else()
	enable_language(ASM)
endif()

if (WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:no")
	if (ARCH STREQUAL "i386")
        list(APPEND SRCS "asm-impl/coroutine-win-i386.asm")
    else()
        list(APPEND SRCS "asm-impl/coroutine-win-x64.asm")
    endif()
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

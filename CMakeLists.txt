cmake_minimum_required(VERSION 3.25)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#set(BUILD_EXAMPLE ON)

# Set build type to DEBUG by default if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "Choose the type of build (DEBUG, RELEASE)" FORCE)
endif()

# Define macros for different build configurations
if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    add_definitions(-DWL_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    add_definitions(-DWL_RELEASE)
elseif(CMAKE_BUILD_TYPE STREQUAL "DIST")
    add_definitions(-DWL_DIST)
endif()

# Define macros for different operating system
# if CMAKE_SYSTEM_NAME is empty, add -DCMAKE_SYSTEM_NAME=Windows in cmake command line.
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("Configuring for Windows")
    add_definitions(-DWL_PLATFORM_WINDOWS)
    # Add Linux-specific configurations
else()
    message("Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()

project(ImguiOpenGL LANGUAGES C CXX)

add_subdirectory(vendor)
add_subdirectory(gui)
if(${BUILD_EXAMPLE} )
    message(${BUILD_EXAMPLE})
    add_subdirectory(app)
endif ()
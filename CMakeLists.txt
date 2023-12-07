cmake_minimum_required(VERSION 3.25)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(BUILD_OBJECT_LIBS OFF)
set(BUILD_TESTING OFF)

# Set build type to DEBUG by default if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "Choose the type of build (DEBUG, RELEASE)" FORCE)
endif()


project(Cpp-but LANGUAGES C CXX)

add_subdirectory(vendor)
add_subdirectory(gui)
add_subdirectory(app)

# necessary files
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_SOURCE_DIR}/gamestate_integration_cs2.cfg ${CMAKE_CURRENT_SOURCE_DIR}/HOW_TO_USE.txt ${CMAKE_CURRENT_SOURCE_DIR}/settings.json DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# copy the test steamappid to the bin folder only during development
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Debug Mode")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Release Mode")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
elseif(CMAKE_BUILD_TYPE STREQUAL "Dist")
    message("Dist Mode")

endif()

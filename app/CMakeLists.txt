file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_executable(CS-but ${srcs})
target_link_libraries(CS-but PUBLIC GGgui)
target_link_libraries(CS-but PUBLIC httplib)
target_link_libraries(CS-but PUBLIC jsoncpp_static)

# Set the path to the Steamworks SDK
set(STEAMWORKS_SDK_PATH ${CMAKE_SOURCE_DIR}/vendor/steamworks_sdk)
# Include directories for Steamworks SDK
include_directories(${STEAMWORKS_SDK_PATH}/public)
# Link against the Steamworks library
target_link_libraries(CS-but PUBLIC ${STEAMWORKS_SDK_PATH}/redistributable_bin/win64/steam_api64.lib)
file(COPY ${STEAMWORKS_SDK_PATH}/redistributable_bin/win64/steam_api64.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


target_link_libraries(CS-but PUBLIC )

get_target_property(JSON_INC_PATH jsoncpp_static INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${JSON_INC_PATH})
set_target_properties(CS-but PROPERTIES WIN32_EXECUTABLE TRUE)

target_include_directories(CS-but PUBLIC include)
#target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/vendor/boost)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_executable(CS-but ${srcs})
target_link_libraries(CS-but PUBLIC GGgui)
target_link_libraries(CS-but PUBLIC httplib)

target_link_libraries(CS-but PUBLIC jsoncpp_static)
get_target_property(JSON_INC_PATH jsoncpp_static INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${JSON_INC_PATH})
#set_target_properties(CS-but PROPERTIES WIN32_EXECUTABLE TRUE)

target_include_directories(CS-but PUBLIC include)
#target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/vendor/boost)

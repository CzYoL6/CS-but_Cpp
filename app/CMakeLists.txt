file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_executable(app ${srcs})
target_link_libraries(app PUBLIC GGgui)
target_link_libraries(app PUBLIC httplib)

target_link_libraries(GGgui PUBLIC jsoncpp_static)
get_target_property(JSON_INC_PATH jsoncpp_static INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${JSON_INC_PATH})

target_include_directories(app PUBLIC include)
#target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/vendor/boost)

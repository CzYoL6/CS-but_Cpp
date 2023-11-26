file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_executable(app ${srcs})
target_link_libraries(app PUBLIC GGgui)

target_include_directories(app PUBLIC include)
#target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/vendor/boost)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(GGgui STATIC ${srcs})
target_link_libraries(GGgui PUBLIC glfw)
target_link_libraries(GGgui PUBLIC imgui)
target_link_libraries(GGgui PUBLIC glm)
target_link_libraries(GGgui PUBLIC stb_image)
target_link_libraries(GGgui PUBLIC spdlog)
target_link_libraries(GGgui PUBLIC opengl32)
target_link_libraries(GGgui PUBLIC soloud)
target_link_libraries(GGgui PUBLIC httplib)
target_link_libraries(GGgui PUBLIC glu32)


target_include_directories(GGgui PUBLIC include)

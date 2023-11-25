file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_library(imgui STATIC ${srcs})
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(imgui PUBLIC glfw)
target_link_libraries(imgui PUBLIC Dwmapi)
target_include_directories(imgui PUBLIC include)
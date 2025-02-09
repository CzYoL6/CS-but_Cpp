file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_library(stb_image STATIC ${srcs} "src/stb_image.cpp")
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(stb_image PUBLIC include)
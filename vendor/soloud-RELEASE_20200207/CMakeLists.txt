add_compile_definitions(WITH_MINIAUDIO)
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS
        src/audiosource/*.cpp
        src/audiosource/*.c
        src/backend/miniaudio/*.cpp
        src/backend/miniaudio/*.c
        src/c_api/*.cpp
        src/c_api/*.c
        src/core/*.cpp
        src/core/*.c
        src/filter/*.cpp
        src/filter/*.c
        include/*.h
)
add_library(soloud STATIC ${srcs})
#target_link_libraries(soloud PUBLIC stb)
target_include_directories(soloud PUBLIC include)
#target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/vendor/boost)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_PREFIX_PATH "/usr/lib64/")

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-static)

project(kora_audio_core)

# Get header and source file name
file(GLOB_RECURSE SOURCE_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file(GLOB_RECURSE SOURCE_HEADER RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)

# create target
add_library(kora_audio_core STATIC ${SOURCE_CPP})

target_include_directories(kora_audio_core PRIVATE /usr/include/)
target_include_directories(kora_audio_core PRIVATE /usr/include/spa-0.2/)
target_include_directories(kora_audio_core PRIVATE /usr/include/pipewire-0.3/)

target_link_libraries(kora_audio_core PUBLIC pipewire-0.3)
target_link_libraries(kora_audio_core PUBLIC pthread)

install(TARGETS kora_audio_core RUNTIME DESTINATION bin)

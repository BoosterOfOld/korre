cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-static)

project(imtui)

set(CMAKE_EXPORT_COMPILE_COMMANDS "on")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

set(IMTUI_STANDALONE ON)

if (EMSCRIPTEN)
    set(IMTUI_SUPPORT_NCURSES_DEFAULT OFF)
    set(IMTUI_SUPPORT_CURL_DEFAULT OFF)
    set(IMTUI_LIBRARY_TYPE STATIC)
else()
    set(IMTUI_SUPPORT_NCURSES_DEFAULT ON)
    set(IMTUI_SUPPORT_CURL_DEFAULT ON)
    if (BUILD_SHARED_LIBS)
        set(IMTUI_LIBRARY_TYPE SHARED)
    else ()
        set(IMTUI_LIBRARY_TYPE STATIC)
    endif()
endif()

# options

option(IMTUI_ALL_WARNINGS            "imtui: enable all compiler warnings" ${IMTUI_STANDALONE})
option(IMTUI_ALL_WARNINGS_3RD_PARTY  "imtui: enable all compiler warnings in 3rd party libs" OFF)

option(IMTUI_SANITIZE_THREAD         "imtui: enable thread sanitizer" OFF)
option(IMTUI_SANITIZE_ADDRESS        "imtui: enable address sanitizer" OFF)
option(IMTUI_SANITIZE_UNDEFINED      "imtui: enable undefined sanitizer" OFF)

option(IMTUI_SUPPORT_NCURSES         "imtui: support for libncurses" ${IMTUI_SUPPORT_NCURSES_DEFAULT})
option(IMTUI_SUPPORT_CURL            "imtui: support for libcurl" ${IMTUI_SUPPORT_CURL_DEFAULT})

option(IMTUI_BUILD_EXAMPLES          "imtui: build examples" ${IMTUI_STANDALONE})

# sanitizers

if (IMTUI_SANITIZE_THREAD)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif()

if (IMTUI_SANITIZE_ADDRESS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif()

if (IMTUI_SANITIZE_UNDEFINED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

# dependencies

if (IMTUI_SUPPORT_NCURSES)
    if (MINGW)
        set(CURSES_LIBRARIES pdcurses)
    else()

        find_package(Curses REQUIRED)
        CHECK_LIBRARY_EXISTS("${CURSES_NCURSES_LIBRARY}"
            nodelay "" CURSES_NCURSES_HAS_NODELAY)
        if(NOT CURSES_NCURSES_HAS_NODELAY)
            find_library(CURSES_EXTRA_LIBRARY tinfo)
            CHECK_LIBRARY_EXISTS("${CURSES_EXTRA_LIBRARY}"
                nodelay "" CURSES_TINFO_HAS_NODELAY)
        endif()
        if(CURSES_EXTRA_LIBRARY)
            set(CURSES_LIBRARIES ${CURSES_LIBRARIES} ${CURSES_EXTRA_LIBRARY})
        endif()
    endif()
endif()

# main

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif ()

add_subdirectory(imtui/third-party)

if (IMTUI_ALL_WARNINGS)
    if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    else()
        # todo : windows
    endif()
endif()

add_subdirectory(imtui/src)


project(kora)


#set(BUILD_SHARED_LIBS OFF)

add_executable(kora
        main.cpp
        imtui/src/imtui-impl-ncurses.cpp
        pffft/pffft.h
        pffft/pffft.c
        pffft/pfft.h
        include/tukey.h
        include/elements/player.h
        src/elements/player.cpp
        src/tukey.cpp
        include/sinks/portaudio_sink.h
        src/sinks/portaudio_sink.cpp
        include/sinks/sink.h
        src/sinks/sink.cpp
        include/sampler.h
        src/sampler.cpp
        include/sources/internal_signal.h
        src/sources/internal_signal.cpp
        include/sources/wave.h
        src/sources/wave.cpp
        include/sources/wave_source.h
        src/sources/wave_source.cpp
        include/decoders/flac_decode.h
        src/decoders/flac_decode.cpp
        include/elements/meter.h
        src/elements/meter.cpp
        src/elements/audio_select.cpp
        include/elements/audio_select.h
        include/elements/dsp_convolver.h
        src/elements/dsp_convolver.cpp
        include/elements/queue.h
        src/elements/queue.cpp
        include/elements/korre.h
        src/elements/korre.cpp
        include/windowth.h
        src/windowth.cpp
        src/decoders/minimp3.h
        src/decoders/mp3_decode.cpp
        src/decoders/minimp3_ex.h
        include/decoders/mp3_decode.h)

include_directories(./)
include_directories(/usr/local/include)
include_directories(./imtui)
include_directories(./include)
include_directories(/opt/homebrew/include)

#set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".so" ".dylib")
find_library(l_portaudio NAMES portaudio)
find_library(l_ncurses NAMES ncursesw)
find_library(l_armadillo NAMES armadillo)
find_library(l_flac NAMES flac)
find_library(l_flacpp NAMES flac++)

target_link_libraries(kora PUBLIC ${l_portaudio})
target_link_libraries(kora PUBLIC ${l_ncurses})
target_link_libraries(kora PUBLIC ${l_armadillo})
target_link_libraries(kora PUBLIC imtui)
target_link_libraries(kora PUBLIC ${l_flac})
target_link_libraries(kora PUBLIC ${l_flacpp})

message(MSG_PORTAUDIO="${l_portaudio}")
message(MSG_NCURSES="${l_ncurses}")
message(MSG_ARMADILLO="${l_armadillo}")
message(MSG_FLAC="${l_flac}")
message(MSG_FLAC="${l_flacpp}")

install(TARGETS kora RUNTIME DESTINATION bin)

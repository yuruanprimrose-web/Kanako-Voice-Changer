cmake_minimum_required(VERSION 3.21)
project(KanakoVoiceChanger VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PA_BUILD_SHARED_LIBS OFF CACHE BOOL "Build PortAudio as a static library" FORCE)
set(PA_BUILD_TESTS OFF CACHE BOOL "Do not build PortAudio tests" FORCE)
set(PA_BUILD_EXAMPLES OFF CACHE BOOL "Do not build PortAudio examples" FORCE)
set(PA_USE_ASIO OFF CACHE BOOL "Do not fetch the separately licensed ASIO SDK" FORCE)

include(FetchContent)
find_package(Qt6 6.8 REQUIRED COMPONENTS Widgets)

FetchContent_Declare(portaudio
    GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
    GIT_TAG v19.7.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(portaudio)

if(TARGET portaudio_static)
    set(KANAKO_PORTAUDIO_TARGET portaudio_static)
elseif(TARGET portaudio)
    set(KANAKO_PORTAUDIO_TARGET portaudio)
else()
    message(FATAL_ERROR "PortAudio did not provide a known CMake library target.")
endif()

qt_standard_project_setup()
qt_add_executable(KanakoVoiceChanger WIN32
    app/src/main.cpp
    app/src/mainwindow.cpp
    app/src/mainwindow.h
)
target_link_libraries(KanakoVoiceChanger PRIVATE Qt6::Widgets ${KANAKO_PORTAUDIO_TARGET})
target_include_directories(KanakoVoiceChanger PRIVATE app/src)
if(MSVC)
    target_compile_options(KanakoVoiceChanger PRIVATE /W4 /permissive-)
else()
    target_compile_options(KanakoVoiceChanger PRIVATE -Wall -Wextra -Wpedantic)
endif()
set_target_properties(KanakoVoiceChanger PROPERTIES
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "Kanako Voice Changer"
)

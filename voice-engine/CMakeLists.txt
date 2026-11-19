add_library(kanako_voice_engine STATIC
    voiceprocessor.cpp
    voiceprocessor.h
)
target_include_directories(kanako_voice_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(kanako_voice_engine PUBLIC signalsmith-stretch)
target_compile_features(kanako_voice_engine PUBLIC cxx_std_17)

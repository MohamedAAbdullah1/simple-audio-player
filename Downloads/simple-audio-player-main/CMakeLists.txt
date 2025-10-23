cmake_minimum_required(VERSION 3.15)
project(test VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 14)

# Add JUCE
add_subdirectory(JUCE)

juce_add_gui_app(test
        PRODUCT_NAME "Simple Audio Player"
        COMPANY_NAME "YourCompany")

target_sources(test
        PRIVATE
        Source/Main.cpp
        Source/MainComponent.cpp
        Source/MainComponent.h
        Source/PlayerAudio.cpp
        Source/PlayerAudio.h
        Source/PlayerGUI.cpp
        Source/PlayerGUI.h)

target_compile_definitions(test
        PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:test,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:test,JUCE_VERSION>")

target_link_libraries(test
        PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
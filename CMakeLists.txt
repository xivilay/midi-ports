cmake_minimum_required(VERSION 3.1.5)

set(PROJECT_NAME "MidiPorts")

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(PROJECT_NAME "${PROJECT_NAME}Dev")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VERSION "0.0.1")
set(JUCE_DIR "../JUCE/")
set(SRC_DIR "./src")

project(${PROJECT_NAME} VERSION ${VERSION})

add_subdirectory(${JUCE_DIR} "juce")

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME XivilaY
    PRODUCT_NAME "${PROJECT_NAME}"
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    FORMATS VST3 Standalone
)

juce_generate_juce_header(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    "${SRC_DIR}/Main.cpp"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_USE_WINRT_MIDI=1
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags

    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)
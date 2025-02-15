cmake_minimum_required(VERSION 3.1.5)

set(PROJECT_NAME "MidiPorts")

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(PROJECT_NAME "${PROJECT_NAME}Dev")
endif()

set(VERSION "0.1.1")

set(FORMATS "Standalone" "AU" "LV2" "VST3")

set(SRC_DIR "./src")
set(JUCE_DIR "./external/juce/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PROJECT_NAME} VERSION ${VERSION})

add_subdirectory(${JUCE_DIR} "juce")

if(WIN32)
    message(STATUS "Building for Windows...")
    set(ASIO_DIR "sdk/asio")
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${ASIO_DIR})
        set(BUILD_ASIO 1)
    endif()
endif()

set(VST2_DIR "sdk/vst2")
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${VST2_DIR})
    set(BUILD_VST2 1)
    juce_set_vst2_sdk_path(${VST2_DIR})
    list(APPEND FORMATS "VST")
endif()

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME XivilaY
    PRODUCT_NAME "${PROJECT_NAME}"
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    PLUGIN_MANUFACTURER_CODE XivY
    PLUGIN_CODE Xiv5
    LV2URI "https://github.com/xivilay/midi-ports"
    VST3_CATEGORIES "Tools"
    FORMATS ${FORMATS}
    BLUETOOTH_PERMISSION_ENABLED TRUE 
    BLUETOOTH_PERMISSION_TEXT "Needed for bluetooth midi input devices"
)

juce_generate_juce_header(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    "${SRC_DIR}/Main.cpp"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    JUCE_USE_WINRT_MIDI=1
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
    ) 
    add_link_options("-Wl,-weak_reference_mismatches,weak")
endif()

if(BUILD_VST2)
    message(STATUS "Adding VST2...")
    target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_VST3_CAN_REPLACE_VST2=1)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0)
endif(BUILD_VST2)

if(BUILD_ASIO)
    message(STATUS "Adding ASIO...")
    target_compile_definitions(${PROJECT_NAME} PRIVATE JUCE_ASIO=1)
    target_include_directories(${PROJECT_NAME} PRIVATE ${ASIO_DIR})
endif(BUILD_ASIO)

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
)
cmake_minimum_required(VERSION 3.24)
project(WavetableSynth VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT JUCE_DIR)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt)
        set(JUCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/JUCE)
    elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/juce-8.0.12-windows/JUCE/CMakeLists.txt)
        set(JUCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/juce-8.0.12-windows/JUCE)
    else()
        message(FATAL_ERROR "JUCE not found. Download JUCE and place it in a 'JUCE' subdirectory, or set -DJUCE_DIR=path/to/JUCE")
    endif()
endif()

add_subdirectory(${JUCE_DIR})

juce_add_plugin(WavetableSynth
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Your
    PLUGIN_CODE WaSy
    VERSION 1.0.0
    FORMATS VST3
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    EDITOR_NEEDS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(WavetableSynth PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/SynthVoice.h
    Source/SynthVoice.cpp
    Source/SynthSound.h
    Source/SynthSound.cpp
    Source/WavetableOscillator.h
    Source/WavetableOscillator.cpp
    Source/Filter.h
    Source/Filter.cpp
    Source/Envelope.h
    Source/Envelope.cpp
    Source/ModMatrix.h
    Source/ModMatrix.cpp
    Source/LookAndFeel.h
    Source/LookAndFeel.cpp
)

target_link_libraries(WavetableSynth PRIVATE
    juce::juce_dsp
    juce::juce_audio_utils
    juce::juce_gui_extra
)

target_compile_definitions(WavetableSynth PRIVATE
    JucePlugin_Name="WavetableSynth"
    JucePlugin_Desc="A wavetable synthesizer with modulation"
    JucePlugin_Manufacturer="YourCompany"
    JucePlugin_ManufacturerCode=0x596F7572
    JucePlugin_PluginCode=0x57615379
)

if(MSVC)
    target_compile_options(WavetableSynth PRIVATE $<$<CONFIG:Debug>:/Zi>)
    target_link_options(WavetableSynth PRIVATE $<$<CONFIG:Debug>:/DEBUG:FULL>)
endif()

cmake_minimum_required(VERSION 3.15)
project(ToneTrigger VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JUCE
find_package(JUCE REQUIRED)

# Set JUCE modules
juce_add_gui_app(ToneTrigger
    PRODUCT_NAME "ToneTrigger"
    COMPANY_NAME "ToneTrigger"
    COMPANY_WEBSITE "https://github.com/makalin/ToneTrigger"
    COMPANY_EMAIL "makalin@gmail.com"
    BUNDLE_ID "com.toneTrigger.app"
    VERSION "1.0.0"
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.png"
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_PLUGIN FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES "Fx"
    AU_MAIN_TYPE "kAudioUnitType_Effect"
    AAX_IDENTIFIER "com.toneTrigger.aax"
    VST3_DESCRIPTION "ToneTrigger - Live Guitar Effect Processor"
    AU_DESCRIPTION "ToneTrigger - Live Guitar Effect Processor"
    AAX_DESCRIPTION "ToneTrigger - Live Guitar Effect Processor"
)

# Add JUCE modules
target_compile_definitions(ToneTrigger
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_LOAD_CURL_SYMBOLS_LAZILY=1
)

target_link_libraries(ToneTrigger
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_formats
    juce::juce_audio_devices
    juce::juce_audio_basics
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_opengl
    juce::juce_audio_plugin_client
)

       # Add source files
       target_sources(ToneTrigger
           PRIVATE
               src/main.cpp
               src/MainComponent.cpp
               src/MainComponent.h
               src/AudioProcessor.cpp
               src/AudioProcessor.h
               src/TriggerManager.cpp
               src/TriggerManager.h
               src/EffectProcessor.cpp
               src/EffectProcessor.h
               src/AudioAnalyzer.cpp
               src/AudioAnalyzer.h
               src/NoteDetector.cpp
               src/NoteDetector.h
               src/ChordDetector.cpp
               src/ChordDetector.h
               src/MelodyDetector.cpp
               src/MelodyDetector.h
               src/MidiProcessor.cpp
               src/MidiProcessor.h
               src/Effects/DistortionEffect.cpp
               src/Effects/DistortionEffect.h
               src/Effects/ReverbEffect.cpp
               src/Effects/ReverbEffect.h
               src/Effects/DelayEffect.cpp
               src/Effects/DelayEffect.h
               src/Effects/ChorusEffect.cpp
               src/Effects/ChorusEffect.h
               src/Effects/FilterEffect.cpp
               src/Effects/FilterEffect.h
               src/Effects/CompressorEffect.cpp
               src/Effects/CompressorEffect.h
               src/UI/TriggerPanel.cpp
               src/UI/TriggerPanel.h
               src/UI/EffectPanel.cpp
               src/UI/EffectPanel.h
               src/UI/AudioSettingsPanel.cpp
               src/UI/AudioSettingsPanel.h
               src/UI/AudioVisualizer.cpp
               src/UI/AudioVisualizer.h
               src/UI/MidiSettingsPanel.cpp
               src/UI/MidiSettingsPanel.h
               src/Utils/AudioUtils.cpp
               src/Utils/AudioUtils.h
               src/Utils/ConfigManager.cpp
               src/Utils/ConfigManager.h
               src/Utils/PresetManager.cpp
               src/Utils/PresetManager.h
       )

# Include directories
target_include_directories(ToneTrigger
    PRIVATE
        src
        src/Effects
        src/UI
        src/Utils
)

# Platform-specific settings
if(APPLE)
    target_link_libraries(ToneTrigger
        "-framework CoreAudio"
        "-framework CoreFoundation"
        "-framework AudioToolbox"
        "-framework AVFoundation"
    )
elseif(WIN32)
    target_link_libraries(ToneTrigger
        ole32
        oleaut32
        uuid
        winmm
        ws2_32
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(ToneTrigger
        asound
        X11
        Xext
        Xinerama
    )
endif()

# Set output directory
set_target_properties(ToneTrigger PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Install target
install(TARGETS ToneTrigger
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
) 
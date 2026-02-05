cmake_minimum_required(VERSION 3.15)

project(SquareBeats VERSION 1.0.0)

# Add JUCE as a subdirectory (assumes JUCE is available)
# Users should set JUCE_DIR to their JUCE installation path
if(NOT DEFINED JUCE_DIR)
    message(STATUS "JUCE_DIR not set. Please set it to your JUCE installation path.")
    message(STATUS "Example: cmake -DJUCE_DIR=/path/to/JUCE ..")
    set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE" CACHE PATH "Path to JUCE framework")
endif()

if(EXISTS "${JUCE_DIR}/CMakeLists.txt")
    add_subdirectory(${JUCE_DIR} JUCE)
else()
    message(FATAL_ERROR "JUCE not found at ${JUCE_DIR}. Please install JUCE or set JUCE_DIR correctly.")
endif()

# Create the plugin target
# SquareBeats is a MIDI sequencer that generates MIDI output
# Configured as a synth with audio output for Ableton Live compatibility
# (Ableton doesn't properly support pure MIDI effect plugins)
juce_add_plugin(SquareBeats
    COMPANY_NAME "SquareBeats"
    PLUGIN_MANUFACTURER_CODE SqBt
    PLUGIN_CODE SqBe
    FORMATS VST3 Standalone
    PRODUCT_NAME "SquareBeats"
    IS_SYNTH TRUE
    IS_MIDI_EFFECT FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES "Instrument|Generator"
)

# Add binary resources (logo image)
juce_add_binary_data(SquareBeatsData
    SOURCES
        images/logo.png
)

# Add source files
target_sources(SquareBeats
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/PatternModel.cpp
        Source/MIDIGenerator.cpp
        Source/PlaybackEngine.cpp
        Source/StateManager.cpp
        Source/SequencingPlaneComponent.cpp
        Source/ColorSelectorComponent.cpp
        Source/ColorConfigPanel.cpp
        Source/LoopLengthSelector.cpp
        Source/TimeSignatureControls.cpp
        Source/ScaleControls.cpp
        Source/ControlButtons.cpp
        Source/PitchSequencerComponent.cpp
        Source/PlayModeControls.cpp
        Source/ScaleSequencerComponent.cpp
        Source/GateFlashOverlay.cpp
        Source/HelpAboutDialog.cpp
)

# Add header files (for IDE organization)
target_sources(SquareBeats
    PRIVATE
        Source/PluginProcessor.h
        Source/PluginEditor.h
        Source/DataStructures.h
        Source/PatternModel.h
        Source/ConversionUtils.h
        Source/MIDIGenerator.h
        Source/PlaybackEngine.h
        Source/StateManager.h
        Source/SequencingPlaneComponent.h
        Source/ColorSelectorComponent.h
        Source/ColorConfigPanel.h
        Source/LoopLengthSelector.h
        Source/TimeSignatureControls.h
        Source/ScaleControls.h
        Source/ControlButtons.h
        Source/PitchSequencerComponent.h
        Source/PlayModeControls.h
        Source/ScaleSequencerComponent.h
        Source/VisualFeedback.h
        Source/GateFlashOverlay.h
        Source/HelpAboutDialog.h
        Source/AppFont.h
)

# Link JUCE modules
target_link_libraries(SquareBeats
    PRIVATE
        SquareBeatsData
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
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compiler definitions
target_compile_definitions(SquareBeats
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Include directories
target_include_directories(SquareBeats PRIVATE Source)

# Set C++ standard
target_compile_features(SquareBeats PRIVATE cxx_std_17)

# Optional: Build unit tests
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    # Create test executable for PatternModel
    add_executable(PatternModelTests
        Source/PatternModel.test.cpp
        Source/PatternModel.cpp
    )
    
    # Link JUCE core for basic utilities
    target_link_libraries(PatternModelTests
        PRIVATE
            juce::juce_core
            juce::juce_graphics
    )
    
    target_compile_features(PatternModelTests PRIVATE cxx_std_17)
    
    # Include directories
    target_include_directories(PatternModelTests PRIVATE Source)
    
    # Create test executable for ConversionUtils
    add_executable(ConversionUtilsTests
        Source/ConversionUtils.test.cpp
    )
    
    # Link JUCE core for basic utilities
    target_link_libraries(ConversionUtilsTests
        PRIVATE
            juce::juce_core
    )
    
    target_compile_features(ConversionUtilsTests PRIVATE cxx_std_17)
    
    # Include directories
    target_include_directories(ConversionUtilsTests PRIVATE Source)
    
    # Create test executable for MIDIGenerator
    add_executable(MIDIGeneratorTests
        Source/MIDIGenerator.test.cpp
        Source/MIDIGenerator.cpp
    )
    
    # Link JUCE modules needed for MIDI
    target_link_libraries(MIDIGeneratorTests
        PRIVATE
            juce::juce_core
            juce::juce_audio_basics
    )
    
    target_compile_features(MIDIGeneratorTests PRIVATE cxx_std_17)
    
    # Include directories
    target_include_directories(MIDIGeneratorTests PRIVATE Source)
    
    # Create test executable for PlaybackEngine
    add_executable(PlaybackEngineTests
        Source/PlaybackEngine.test.cpp
        Source/PlaybackEngine.cpp
        Source/PatternModel.cpp
        Source/MIDIGenerator.cpp
    )
    
    # Link JUCE modules needed for PlaybackEngine
    target_link_libraries(PlaybackEngineTests
        PRIVATE
            juce::juce_core
            juce::juce_audio_basics
            juce::juce_graphics
    )
    
    target_compile_features(PlaybackEngineTests PRIVATE cxx_std_17)
    
    # Include directories
    target_include_directories(PlaybackEngineTests PRIVATE Source)
    
    # Create test executable for StateManager
    add_executable(StateManagerTests
        Source/StateManager.test.cpp
        Source/StateManager.cpp
        Source/PatternModel.cpp
    )
    
    # Link JUCE modules needed for StateManager
    target_link_libraries(StateManagerTests
        PRIVATE
            juce::juce_core
            juce::juce_graphics
    )
    
    target_compile_features(StateManagerTests PRIVATE cxx_std_17)
    
    # Include directories
    target_include_directories(StateManagerTests PRIVATE Source)
    
    # Create test executable for SequencingPlaneComponent
    add_executable(SequencingPlaneComponentTests
        Source/SequencingPlaneComponent.test.cpp
        Source/SequencingPlaneComponent.cpp
        Source/PatternModel.cpp
    )
    
    # Link JUCE modules needed for SequencingPlaneComponent
    target_link_libraries(SequencingPlaneComponentTests
        PRIVATE
            juce::juce_core
            juce::juce_graphics
            juce::juce_gui_basics
            juce::juce_events
    )
    
    target_compile_features(SequencingPlaneComponentTests PRIVATE cxx_std_17)
    
    # Include directories
    target_include_directories(SequencingPlaneComponentTests PRIVATE Source)
    
    message(STATUS "Unit tests enabled. Build targets: PatternModelTests, ConversionUtilsTests, MIDIGeneratorTests, PlaybackEngineTests, StateManagerTests, SequencingPlaneComponentTests")
endif()

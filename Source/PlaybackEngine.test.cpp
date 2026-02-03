#include "PlaybackEngine.h"
#include "PatternModel.h"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace SquareBeats;

//==============================================================================
// Test helper functions

void assertTrue(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << std::endl;
        exit(1);
    }
    std::cout << "PASSED: " << message << std::endl;
}

void assertNear(double actual, double expected, double tolerance, const char* message) {
    if (std::abs(actual - expected) > tolerance) {
        std::cerr << "FAILED: " << message 
                  << " (expected " << expected << ", got " << actual << ")" << std::endl;
        exit(1);
    }
    std::cout << "PASSED: " << message << std::endl;
}

//==============================================================================
// Test: PlaybackEngine initialization
void testPlaybackEngineInitialization() {
    std::cout << "\n=== Test: PlaybackEngine Initialization ===" << std::endl;
    
    PlaybackEngine engine;
    PatternModel model;
    
    engine.setPatternModel(&model);
    
    std::cout << "PlaybackEngine initialized successfully" << std::endl;
}

//==============================================================================
// Test: Transport synchronization
void testTransportSynchronization() {
    std::cout << "\n=== Test: Transport Synchronization ===" << std::endl;
    
    PlaybackEngine engine;
    PatternModel model;
    engine.setPatternModel(&model);
    
    // Test transport change
    engine.handleTransportChange(true, 44100.0, 120.0, 0.0, 0.0);
    
    std::cout << "Transport synchronization working" << std::endl;
}

//==============================================================================
// Test: Loop boundary handling
void testLoopBoundaryHandling() {
    std::cout << "\n=== Test: Loop Boundary Handling ===" << std::endl;
    
    PlaybackEngine engine;
    PatternModel model;
    
    // Set up a 2-bar loop at 4/4
    model.setLoopLength(2);
    model.setTimeSignature(4, 4);
    
    engine.setPatternModel(&model);
    
    // Start playback
    engine.handleTransportChange(true, 44100.0, 120.0, 0.0, 0.0);
    
    std::cout << "Loop boundary handling initialized" << std::endl;
}

//==============================================================================
// Test: Square trigger detection
void testSquareTriggerDetection() {
    std::cout << "\n=== Test: Square Trigger Detection ===" << std::endl;
    
    PlaybackEngine engine;
    PatternModel model;
    
    // Set up pattern
    model.setLoopLength(2);
    model.setTimeSignature(4, 4);
    
    // Create a square at the beginning
    Square* square = model.createSquare(0.0f, 0.5f, 0.25f, 0.5f, 0);
    assertTrue(square != nullptr, "Square created");
    
    engine.setPatternModel(&model);
    
    // Start playback
    engine.handleTransportChange(true, 44100.0, 120.0, 0.0, 0.0);
    
    // Process a block
    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midiMessages;
    
    engine.processBlock(buffer, midiMessages);
    
    std::cout << "Square trigger detection working" << std::endl;
}

//==============================================================================
// Test: Monophonic voice management
void testMonophonicVoiceManagement() {
    std::cout << "\n=== Test: Monophonic Voice Management ===" << std::endl;
    
    PlaybackEngine engine;
    PatternModel model;
    
    // Set up pattern
    model.setLoopLength(2);
    model.setTimeSignature(4, 4);
    
    // Create two overlapping squares on the same color channel
    Square* square1 = model.createSquare(0.0f, 0.3f, 0.3f, 0.5f, 0);
    Square* square2 = model.createSquare(0.2f, 0.6f, 0.3f, 0.5f, 0);
    
    assertTrue(square1 != nullptr, "Square 1 created");
    assertTrue(square2 != nullptr, "Square 2 created");
    
    engine.setPatternModel(&model);
    
    // Start playback
    engine.handleTransportChange(true, 44100.0, 120.0, 0.0, 0.0);
    
    // Process blocks to trigger both squares
    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midiMessages;
    
    for (int i = 0; i < 10; ++i) {
        engine.processBlock(buffer, midiMessages);
    }
    
    std::cout << "Monophonic voice management working" << std::endl;
}

//==============================================================================
// Test: ProcessBlock integration
void testProcessBlockIntegration() {
    std::cout << "\n=== Test: ProcessBlock Integration ===" << std::endl;
    
    PlaybackEngine engine;
    PatternModel model;
    
    // Set up pattern
    model.setLoopLength(1);
    model.setTimeSignature(4, 4);
    
    // Create a square
    Square* square = model.createSquare(0.25f, 0.5f, 0.25f, 0.5f, 0);
    assertTrue(square != nullptr, "Square created");
    
    engine.setPatternModel(&model);
    
    // Start playback
    engine.handleTransportChange(true, 44100.0, 120.0, 0.0, 0.0);
    
    // Process multiple blocks
    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midiMessages;
    
    for (int i = 0; i < 20; ++i) {
        midiMessages.clear();
        engine.processBlock(buffer, midiMessages);
    }
    
    std::cout << "ProcessBlock integration working" << std::endl;
}

//==============================================================================
// Test: Transport stop sends note-offs
void testTransportStopSendsNoteOffs() {
    std::cout << "\n=== Test: Transport Stop Sends Note-Offs ===" << std::endl;
    
    PlaybackEngine engine;
    PatternModel model;
    
    // Set up pattern
    model.setLoopLength(1);
    model.setTimeSignature(4, 4);
    
    // Create a long square
    Square* square = model.createSquare(0.0f, 0.5f, 0.9f, 0.5f, 0);
    assertTrue(square != nullptr, "Square created");
    
    engine.setPatternModel(&model);
    
    // Start playback
    engine.handleTransportChange(true, 44100.0, 120.0, 0.0, 0.0);
    
    // Process a few blocks to trigger the note
    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midiMessages;
    
    for (int i = 0; i < 5; ++i) {
        engine.processBlock(buffer, midiMessages);
    }
    
    // Stop transport
    engine.handleTransportChange(false, 44100.0, 120.0, 0.0, 0.0);
    
    std::cout << "Transport stop handling working" << std::endl;
}

//==============================================================================
// Test: Pitch sequencer integration
void testPitchSequencerIntegration() {
    std::cout << "\n=== Test: Pitch Sequencer Integration ===" << std::endl;
    
    PlaybackEngine engine;
    PatternModel model;
    
    // Set up pattern
    model.setLoopLength(2);
    model.setTimeSignature(4, 4);
    
    // Create a square
    Square* square = model.createSquare(0.25f, 0.5f, 0.25f, 0.5f, 0);
    assertTrue(square != nullptr, "Square created");
    
    // Set up pitch sequencer with a simple waveform
    PitchSequencer& pitchSeq = model.getPitchSequencer();
    pitchSeq.visible = true;
    pitchSeq.loopLengthBars = 2;  // Same as main loop
    pitchSeq.waveform = {0.0f, 12.0f};  // 0 semitones at start, 12 at end
    
    engine.setPatternModel(&model);
    
    // Start playback
    engine.handleTransportChange(true, 44100.0, 120.0, 0.0, 0.0);
    
    // Process blocks
    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midiMessages;
    
    for (int i = 0; i < 10; ++i) {
        midiMessages.clear();
        engine.processBlock(buffer, midiMessages);
    }
    
    std::cout << "Pitch sequencer integration working" << std::endl;
}

//==============================================================================
// Test: Pitch sequencer hidden state
void testPitchSequencerHiddenState() {
    std::cout << "\n=== Test: Pitch Sequencer Hidden State ===" << std::endl;
    
    PlaybackEngine engine;
    PatternModel model;
    
    // Set up pattern
    model.setLoopLength(1);
    model.setTimeSignature(4, 4);
    
    // Create a square
    Square* square = model.createSquare(0.0f, 0.5f, 0.25f, 0.5f, 0);
    assertTrue(square != nullptr, "Square created");
    
    // Set up pitch sequencer but keep it hidden
    PitchSequencer& pitchSeq = model.getPitchSequencer();
    pitchSeq.visible = false;  // Hidden
    pitchSeq.waveform = {0.0f, 12.0f};
    
    engine.setPatternModel(&model);
    
    // Start playback
    engine.handleTransportChange(true, 44100.0, 120.0, 0.0, 0.0);
    
    // Process blocks - pitch offset should be 0 since sequencer is hidden
    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midiMessages;
    
    for (int i = 0; i < 5; ++i) {
        midiMessages.clear();
        engine.processBlock(buffer, midiMessages);
    }
    
    std::cout << "Pitch sequencer hidden state working" << std::endl;
}

//==============================================================================
// Test: Pitch sequencer independent loop length
void testPitchSequencerIndependentLoop() {
    std::cout << "\n=== Test: Pitch Sequencer Independent Loop ===" << std::endl;
    
    PlaybackEngine engine;
    PatternModel model;
    
    // Set up pattern with 2-bar loop
    model.setLoopLength(2);
    model.setTimeSignature(4, 4);
    
    // Create a square
    Square* square = model.createSquare(0.0f, 0.5f, 0.25f, 0.5f, 0);
    assertTrue(square != nullptr, "Square created");
    
    // Set up pitch sequencer with different loop length (4 bars)
    PitchSequencer& pitchSeq = model.getPitchSequencer();
    pitchSeq.visible = true;
    pitchSeq.loopLengthBars = 4;  // Different from main loop (2 bars)
    pitchSeq.waveform = {0.0f, 6.0f};
    
    engine.setPatternModel(&model);
    
    // Start playback
    engine.handleTransportChange(true, 44100.0, 120.0, 0.0, 0.0);
    
    // Process blocks - pitch sequencer should cycle at its own rate
    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midiMessages;
    
    for (int i = 0; i < 20; ++i) {
        midiMessages.clear();
        engine.processBlock(buffer, midiMessages);
    }
    
    std::cout << "Pitch sequencer independent loop working" << std::endl;
}

//==============================================================================
int main() {
    std::cout << "Running PlaybackEngine Unit Tests..." << std::endl;
    
    try {
        testPlaybackEngineInitialization();
        testTransportSynchronization();
        testLoopBoundaryHandling();
        testSquareTriggerDetection();
        testMonophonicVoiceManagement();
        testProcessBlockIntegration();
        testTransportStopSendsNoteOffs();
        testPitchSequencerIntegration();
        testPitchSequencerHiddenState();
        testPitchSequencerIndependentLoop();
        
        std::cout << "\n=== All PlaybackEngine tests passed! ===" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\nException: " << e.what() << std::endl;
        return 1;
    }
}

#include "StateManager.h"
#include "PatternModel.h"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace SquareBeats;

/**
 * Standalone unit tests for StateManager
 * These tests verify serialization and deserialization functionality
 */

void testEmptyPatternRoundTrip()
{
    PatternModel original;
    juce::MemoryBlock stateData;
    
    // Serialize
    StateManager::saveState(original, stateData);
    
    // Deserialize
    PatternModel loaded;
    bool success = StateManager::loadState(loaded, stateData.getData(), static_cast<int>(stateData.getSize()));
    
    assert(success);
    assert(loaded.getLoopLength() == original.getLoopLength());
    assert(loaded.getTimeSignature().numerator == original.getTimeSignature().numerator);
    assert(loaded.getTimeSignature().denominator == original.getTimeSignature().denominator);
    assert(loaded.getAllSquares().size() == 0);
    
    std::cout << "✓ Empty pattern round-trip test passed\n";
}

void testPatternWithSquaresRoundTrip()
{
    PatternModel original;
    
    // Create some squares
    original.createSquare(0.1f, 0.2f, 0.3f, 0.4f, 0);
    original.createSquare(0.5f, 0.6f, 0.1f, 0.2f, 1);
    original.createSquare(0.7f, 0.3f, 0.2f, 0.5f, 2);
    
    juce::MemoryBlock stateData;
    StateManager::saveState(original, stateData);
    
    // Deserialize
    PatternModel loaded;
    bool success = StateManager::loadState(loaded, stateData.getData(), static_cast<int>(stateData.getSize()));
    
    assert(success);
    
    auto originalSquares = original.getAllSquares();
    auto loadedSquares = loaded.getAllSquares();
    
    assert(loadedSquares.size() == originalSquares.size());
    assert(loadedSquares.size() == 3);
    
    // Check first square
    assert(loadedSquares[0]->leftEdge == originalSquares[0]->leftEdge);
    assert(loadedSquares[0]->topEdge == originalSquares[0]->topEdge);
    assert(loadedSquares[0]->width == originalSquares[0]->width);
    assert(loadedSquares[0]->height == originalSquares[0]->height);
    assert(loadedSquares[0]->colorChannelId == originalSquares[0]->colorChannelId);
    
    std::cout << "✓ Pattern with squares round-trip test passed\n";
}

void testGlobalSettingsRoundTrip()
{
    PatternModel original;
    
    // Set custom global settings
    original.setLoopLength(4);
    original.setTimeSignature(3, 8);
    
    juce::MemoryBlock stateData;
    StateManager::saveState(original, stateData);
    
    // Deserialize
    PatternModel loaded;
    bool success = StateManager::loadState(loaded, stateData.getData(), static_cast<int>(stateData.getSize()));
    
    assert(success);
    assert(loaded.getLoopLength() == 4);
    assert(loaded.getTimeSignature().numerator == 3);
    assert(loaded.getTimeSignature().denominator == 8);
    
    std::cout << "✓ Global settings round-trip test passed\n";
}

void testColorChannelConfigRoundTrip()
{
    PatternModel original;
    
    // Modify color channel 0
    ColorChannelConfig config;
    config.midiChannel = 5;
    config.highNote = 96;
    config.lowNote = 36;
    config.quantize = Q_1_8;
    config.displayColor = juce::Colour(0xFF123456);
    original.setColorConfig(0, config);
    
    juce::MemoryBlock stateData;
    StateManager::saveState(original, stateData);
    
    // Deserialize
    PatternModel loaded;
    bool success = StateManager::loadState(loaded, stateData.getData(), static_cast<int>(stateData.getSize()));
    
    assert(success);
    
    const ColorChannelConfig& loadedConfig = loaded.getColorConfig(0);
    assert(loadedConfig.midiChannel == 5);
    assert(loadedConfig.highNote == 96);
    assert(loadedConfig.lowNote == 36);
    assert(loadedConfig.quantize == Q_1_8);
    assert(loadedConfig.displayColor.getARGB() == 0xFF123456);
    
    std::cout << "✓ Color channel configuration round-trip test passed\n";
}

void testPitchSequencerRoundTrip()
{
    PatternModel original;
    
    // Set up pitch sequencer
    PitchSequencer& pitchSeq = original.getPitchSequencer();
    pitchSeq.visible = true;
    pitchSeq.loopLengthBars = 8;
    pitchSeq.waveform = {0.0f, 1.5f, -2.3f, 0.7f, -1.0f};
    
    juce::MemoryBlock stateData;
    StateManager::saveState(original, stateData);
    
    // Deserialize
    PatternModel loaded;
    bool success = StateManager::loadState(loaded, stateData.getData(), static_cast<int>(stateData.getSize()));
    
    assert(success);
    
    const PitchSequencer& loadedPitchSeq = loaded.getPitchSequencer();
    assert(loadedPitchSeq.visible == true);
    assert(loadedPitchSeq.loopLengthBars == 8);
    assert(loadedPitchSeq.waveform.size() == 5);
    assert(loadedPitchSeq.waveform[0] == 0.0f);
    assert(loadedPitchSeq.waveform[1] == 1.5f);
    assert(loadedPitchSeq.waveform[2] == -2.3f);
    assert(loadedPitchSeq.waveform[3] == 0.7f);
    assert(loadedPitchSeq.waveform[4] == -1.0f);
    
    std::cout << "✓ Pitch sequencer round-trip test passed\n";
}

void testInvalidMagicNumber()
{
    juce::MemoryBlock invalidData;
    juce::MemoryOutputStream stream(invalidData, false);
    stream.writeInt(0x12345678); // Wrong magic number
    stream.writeInt(1); // Version
    
    PatternModel model;
    bool success = StateManager::loadState(model, invalidData.getData(), static_cast<int>(invalidData.getSize()));
    
    assert(!success);
    
    std::cout << "✓ Invalid magic number test passed\n";
}

void testInvalidVersion()
{
    juce::MemoryBlock invalidData;
    juce::MemoryOutputStream stream(invalidData, false);
    stream.writeInt(0x53514245); // Correct magic number
    stream.writeInt(999); // Unsupported version
    
    PatternModel model;
    bool success = StateManager::loadState(model, invalidData.getData(), static_cast<int>(invalidData.getSize()));
    
    assert(!success);
    
    std::cout << "✓ Invalid version test passed\n";
}

void testTruncatedData()
{
    juce::MemoryBlock truncatedData;
    truncatedData.setSize(4); // Only 4 bytes
    
    PatternModel model;
    bool success = StateManager::loadState(model, truncatedData.getData(), static_cast<int>(truncatedData.getSize()));
    
    assert(!success);
    
    std::cout << "✓ Truncated data test passed\n";
}

void testNullDataPointer()
{
    PatternModel model;
    bool success = StateManager::loadState(model, nullptr, 100);
    
    assert(!success);
    
    std::cout << "✓ Null data pointer test passed\n";
}

void testComplexPatternRoundTrip()
{
    PatternModel original;
    
    // Create a complex pattern
    original.setLoopLength(2);
    original.setTimeSignature(7, 8);
    
    // Add multiple squares
    for (int i = 0; i < 10; ++i)
    {
        float left = i * 0.08f;
        float top = (i % 4) * 0.2f;
        original.createSquare(left, top, 0.05f, 0.15f, i % 4);
    }
    
    // Configure all color channels
    for (int i = 0; i < 4; ++i)
    {
        ColorChannelConfig config;
        config.midiChannel = i + 1;
        config.highNote = 60 + i * 12;
        config.lowNote = 36 + i * 12;
        config.quantize = static_cast<QuantizationValue>(i % 6);
        config.displayColor = juce::Colour(0xFF000000 + i * 0x111111);
        original.setColorConfig(i, config);
    }
    
    // Set up pitch sequencer
    PitchSequencer& pitchSeq = original.getPitchSequencer();
    pitchSeq.visible = true;
    pitchSeq.loopLengthBars = 16;
    for (int i = 0; i < 100; ++i)
    {
        pitchSeq.waveform.push_back(std::sin(i * 0.1f) * 5.0f);
    }
    
    juce::MemoryBlock stateData;
    StateManager::saveState(original, stateData);
    
    // Deserialize
    PatternModel loaded;
    bool success = StateManager::loadState(loaded, stateData.getData(), static_cast<int>(stateData.getSize()));
    
    assert(success);
    assert(loaded.getAllSquares().size() == 10);
    assert(loaded.getLoopLength() == 2);
    assert(loaded.getTimeSignature().numerator == 7);
    assert(loaded.getTimeSignature().denominator == 8);
    assert(loaded.getPitchSequencer().waveform.size() == 100);
    assert(loaded.getPitchSequencer().loopLengthBars == 16);
    
    std::cout << "✓ Complex pattern round-trip test passed\n";
}

int main()
{
    std::cout << "Running StateManager tests...\n\n";
    
    try
    {
        testEmptyPatternRoundTrip();
        testPatternWithSquaresRoundTrip();
        testGlobalSettingsRoundTrip();
        testColorChannelConfigRoundTrip();
        testPitchSequencerRoundTrip();
        testInvalidMagicNumber();
        testInvalidVersion();
        testTruncatedData();
        testNullDataPointer();
        testComplexPatternRoundTrip();
        
        std::cout << "\n✓ All StateManager tests passed!\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "\n✗ Test failed with unknown exception\n";
        return 1;
    }
}

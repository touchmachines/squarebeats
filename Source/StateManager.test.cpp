#include <catch2/catch_test_macros.hpp>
#include "StateManager.h"
#include "PatternModel.h"

using namespace SquareBeats;

TEST_CASE("StateManager serialization and deserialization", "[StateManager]")
{
    SECTION("Empty pattern round-trip")
    {
        PatternModel original;
        juce::MemoryBlock stateData;
        
        // Serialize
        StateManager::saveState(original, stateData);
        
        // Deserialize
        PatternModel loaded;
        bool success = StateManager::loadState(loaded, stateData.getData(), static_cast<int>(stateData.getSize()));
        
        REQUIRE(success);
        REQUIRE(loaded.getLoopLength() == original.getLoopLength());
        REQUIRE(loaded.getTimeSignature().numerator == original.getTimeSignature().numerator);
        REQUIRE(loaded.getTimeSignature().denominator == original.getTimeSignature().denominator);
        REQUIRE(loaded.getAllSquares().size() == 0);
    }
    
    SECTION("Pattern with squares round-trip")
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
        
        REQUIRE(success);
        
        auto originalSquares = original.getAllSquares();
        auto loadedSquares = loaded.getAllSquares();
        
        REQUIRE(loadedSquares.size() == originalSquares.size());
        REQUIRE(loadedSquares.size() == 3);
        
        // Check first square
        REQUIRE(loadedSquares[0]->leftEdge == originalSquares[0]->leftEdge);
        REQUIRE(loadedSquares[0]->topEdge == originalSquares[0]->topEdge);
        REQUIRE(loadedSquares[0]->width == originalSquares[0]->width);
        REQUIRE(loadedSquares[0]->height == originalSquares[0]->height);
        REQUIRE(loadedSquares[0]->colorChannelId == originalSquares[0]->colorChannelId);
    }
    
    SECTION("Global settings round-trip")
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
        
        REQUIRE(success);
        REQUIRE(loaded.getLoopLength() == 4);
        REQUIRE(loaded.getTimeSignature().numerator == 3);
        REQUIRE(loaded.getTimeSignature().denominator == 8);
    }
    
    SECTION("Color channel configuration round-trip")
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
        
        REQUIRE(success);
        
        const ColorChannelConfig& loadedConfig = loaded.getColorConfig(0);
        REQUIRE(loadedConfig.midiChannel == 5);
        REQUIRE(loadedConfig.highNote == 96);
        REQUIRE(loadedConfig.lowNote == 36);
        REQUIRE(loadedConfig.quantize == Q_1_8);
        REQUIRE(loadedConfig.displayColor.getARGB() == 0xFF123456);
    }
    
    SECTION("Pitch sequencer round-trip")
    {
        PatternModel original;
        
        // Set up pitch sequencer editing mode
        PitchSequencer& pitchSeq = original.getPitchSequencer();
        pitchSeq.editingPitch = true;
        
        // Set up per-color pitch waveform (waveforms are per-color now)
        ColorChannelConfig& colorConfig = original.getColorConfig(0);
        colorConfig.pitchSeqLoopLengthBars = 8;
        colorConfig.pitchWaveform = {0.0f, 1.5f, -2.3f, 0.7f, -1.0f};
        original.setColorConfig(0, colorConfig);
        
        juce::MemoryBlock stateData;
        StateManager::saveState(original, stateData);
        
        // Deserialize
        PatternModel loaded;
        bool success = StateManager::loadState(loaded, stateData.getData(), static_cast<int>(stateData.getSize()));
        
        REQUIRE(success);
        
        const PitchSequencer& loadedPitchSeq = loaded.getPitchSequencer();
        REQUIRE(loadedPitchSeq.editingPitch == true);
        
        const ColorChannelConfig& loadedColorConfig = loaded.getColorConfig(0);
        REQUIRE(loadedColorConfig.pitchSeqLoopLengthBars == 8);
        REQUIRE(loadedColorConfig.pitchWaveform.size() == 5);
        REQUIRE(loadedColorConfig.pitchWaveform[0] == 0.0f);
        REQUIRE(loadedColorConfig.pitchWaveform[1] == 1.5f);
        REQUIRE(loadedColorConfig.pitchWaveform[2] == -2.3f);
        REQUIRE(loadedColorConfig.pitchWaveform[3] == 0.7f);
        REQUIRE(loadedColorConfig.pitchWaveform[4] == -1.0f);
    }
    
    SECTION("Invalid magic number")
    {
        juce::MemoryBlock invalidData;
        juce::MemoryOutputStream stream(invalidData, false);
        stream.writeInt(0x12345678); // Wrong magic number
        stream.writeInt(1); // Version
        
        PatternModel model;
        bool success = StateManager::loadState(model, invalidData.getData(), static_cast<int>(invalidData.getSize()));
        
        REQUIRE_FALSE(success);
    }
    
    SECTION("Invalid version")
    {
        juce::MemoryBlock invalidData;
        juce::MemoryOutputStream stream(invalidData, false);
        stream.writeInt(0x53514245); // Correct magic number
        stream.writeInt(999); // Unsupported version
        
        PatternModel model;
        bool success = StateManager::loadState(model, invalidData.getData(), static_cast<int>(invalidData.getSize()));
        
        REQUIRE_FALSE(success);
    }
    
    SECTION("Truncated data")
    {
        juce::MemoryBlock truncatedData;
        truncatedData.setSize(4); // Only 4 bytes
        
        PatternModel model;
        bool success = StateManager::loadState(model, truncatedData.getData(), static_cast<int>(truncatedData.getSize()));
        
        REQUIRE_FALSE(success);
    }
    
    SECTION("Null data pointer")
    {
        PatternModel model;
        bool success = StateManager::loadState(model, nullptr, 100);
        
        REQUIRE_FALSE(success);
    }
    
    SECTION("Complex pattern round-trip")
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
        
        // Set up pitch sequencer editing mode
        PitchSequencer& pitchSeq = original.getPitchSequencer();
        pitchSeq.editingPitch = true;
        
        // Set up per-color pitch waveform
        ColorChannelConfig& colorConfig = original.getColorConfig(0);
        colorConfig.pitchSeqLoopLengthBars = 16;
        for (int i = 0; i < 100; ++i)
        {
            colorConfig.pitchWaveform.push_back(std::sin(i * 0.1f) * 5.0f);
        }
        original.setColorConfig(0, colorConfig);
        
        juce::MemoryBlock stateData;
        StateManager::saveState(original, stateData);
        
        // Deserialize
        PatternModel loaded;
        bool success = StateManager::loadState(loaded, stateData.getData(), static_cast<int>(stateData.getSize()));
        
        REQUIRE(success);
        REQUIRE(loaded.getAllSquares().size() == 10);
        REQUIRE(loaded.getLoopLength() == 2);
        REQUIRE(loaded.getTimeSignature().numerator == 7);
        REQUIRE(loaded.getTimeSignature().denominator == 8);
        REQUIRE(loaded.getColorConfig(0).pitchWaveform.size() == 100);
        REQUIRE(loaded.getColorConfig(0).pitchSeqLoopLengthBars == 16);
    }
    
    SECTION("Scale sequencer round-trip")
    {
        PatternModel original;
        
        // Set up scale sequencer with multiple segments
        ScaleSequencerConfig& scaleSeq = original.getScaleSequencer();
        scaleSeq.enabled = true;
        scaleSeq.segments.clear();
        
        // Add some segments
        scaleSeq.segments.push_back(ScaleSequenceSegment(ROOT_C, SCALE_MAJOR, 4));
        scaleSeq.segments.push_back(ScaleSequenceSegment(ROOT_G, SCALE_DORIAN, 2));
        scaleSeq.segments.push_back(ScaleSequenceSegment(ROOT_D, SCALE_MINOR, 8));
        
        juce::MemoryBlock stateData;
        StateManager::saveState(original, stateData);
        
        // Deserialize
        PatternModel loaded;
        bool success = StateManager::loadState(loaded, stateData.getData(), static_cast<int>(stateData.getSize()));
        
        REQUIRE(success);
        
        const ScaleSequencerConfig& loadedScaleSeq = loaded.getScaleSequencer();
        REQUIRE(loadedScaleSeq.enabled == true);
        REQUIRE(loadedScaleSeq.segments.size() == 3);
        
        // Check first segment
        REQUIRE(loadedScaleSeq.segments[0].rootNote == ROOT_C);
        REQUIRE(loadedScaleSeq.segments[0].scaleType == SCALE_MAJOR);
        REQUIRE(loadedScaleSeq.segments[0].lengthBars == 4);
        
        // Check second segment
        REQUIRE(loadedScaleSeq.segments[1].rootNote == ROOT_G);
        REQUIRE(loadedScaleSeq.segments[1].scaleType == SCALE_DORIAN);
        REQUIRE(loadedScaleSeq.segments[1].lengthBars == 2);
        
        // Check third segment
        REQUIRE(loadedScaleSeq.segments[2].rootNote == ROOT_D);
        REQUIRE(loadedScaleSeq.segments[2].scaleType == SCALE_MINOR);
        REQUIRE(loadedScaleSeq.segments[2].lengthBars == 8);
    }
    
    SECTION("Scale sequencer disabled round-trip")
    {
        PatternModel original;
        
        // Set up scale sequencer but keep it disabled
        ScaleSequencerConfig& scaleSeq = original.getScaleSequencer();
        scaleSeq.enabled = false;
        scaleSeq.segments.clear();
        scaleSeq.segments.push_back(ScaleSequenceSegment(ROOT_A, SCALE_BLUES, 16));
        
        juce::MemoryBlock stateData;
        StateManager::saveState(original, stateData);
        
        // Deserialize
        PatternModel loaded;
        bool success = StateManager::loadState(loaded, stateData.getData(), static_cast<int>(stateData.getSize()));
        
        REQUIRE(success);
        
        const ScaleSequencerConfig& loadedScaleSeq = loaded.getScaleSequencer();
        REQUIRE(loadedScaleSeq.enabled == false);
        REQUIRE(loadedScaleSeq.segments.size() == 1);
        REQUIRE(loadedScaleSeq.segments[0].rootNote == ROOT_A);
        REQUIRE(loadedScaleSeq.segments[0].scaleType == SCALE_BLUES);
        REQUIRE(loadedScaleSeq.segments[0].lengthBars == 16);
    }
}


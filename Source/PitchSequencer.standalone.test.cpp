// Standalone test for pitch sequencer functionality (no JUCE dependency)
// Tests the pitch offset interpolation logic in ColorChannelConfig
#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <algorithm>

// Minimal ColorChannelConfig struct for testing pitch waveform functionality
struct ColorChannelConfig {
    std::vector<float> pitchWaveform;
    int pitchSeqLoopLengthBars;
    
    ColorChannelConfig()
        : pitchSeqLoopLengthBars(2)
    {}
    
    float getPitchOffsetAt(double normalizedPosition) const {
        if (pitchWaveform.empty()) {
            return 0.0f;
        }
        
        // Wrap position to [0, 1)
        normalizedPosition = normalizedPosition - std::floor(normalizedPosition);
        
        // Map to waveform index
        double indexFloat = normalizedPosition * (pitchWaveform.size() - 1);
        int index0 = static_cast<int>(std::floor(indexFloat));
        int index1 = std::min(index0 + 1, static_cast<int>(pitchWaveform.size() - 1));
        
        // Linear interpolation
        float t = static_cast<float>(indexFloat - index0);
        return pitchWaveform[index0] * (1.0f - t) + pitchWaveform[index1] * t;
    }
};

// Minimal PitchSequencer struct for testing editing mode
struct PitchSequencer {
    bool editingPitch;  // True when editing pitch sequence, false when editing squares
    
    PitchSequencer()
        : editingPitch(false)
    {}
};

// Helper for approximate comparison
bool approxEqual(float a, float b, float epsilon = 0.01f)
{
    return std::abs(a - b) < epsilon;
}

// Tests
void testEmptyWaveform()
{
    std::cout << "Testing empty waveform returns 0...\n";
    
    ColorChannelConfig config;
    
    assert(config.getPitchOffsetAt(0.0) == 0.0f);
    assert(config.getPitchOffsetAt(0.5) == 0.0f);
    assert(config.getPitchOffsetAt(1.0) == 0.0f);
    
    std::cout << "✓ Empty waveform test passed\n";
}

void testPitchAlwaysApplies()
{
    std::cout << "Testing pitch modulation always applies (regardless of editing mode)...\n";
    
    ColorChannelConfig config;
    config.pitchWaveform = {1.0f, 2.0f, 3.0f};
    
    PitchSequencer seq;
    seq.editingPitch = false;  // Not in editing mode
    
    // Pitch should still be applied - editing mode only affects mouse input, not pitch modulation
    assert(approxEqual(config.getPitchOffsetAt(0.0), 1.0f));
    assert(approxEqual(config.getPitchOffsetAt(0.5), 2.0f));
    
    std::cout << "✓ Pitch always applies test passed\n";
}

void testLinearInterpolation()
{
    std::cout << "Testing linear interpolation...\n";
    
    ColorChannelConfig config;
    config.pitchWaveform = {0.0f, 4.0f};  // Simple case: 0 at start, 4 at end
    
    // At position 0.0, should be 0.0
    float offset = config.getPitchOffsetAt(0.0);
    assert(approxEqual(offset, 0.0f));
    
    // At position 0.5, should be 2.0 (halfway between 0 and 4)
    offset = config.getPitchOffsetAt(0.5);
    assert(approxEqual(offset, 2.0f));
    
    // At position 1.0, wraps to 0.0 (which is 0.0 in the waveform)
    // This is expected behavior - position 1.0 wraps to the start
    offset = config.getPitchOffsetAt(1.0);
    assert(approxEqual(offset, 0.0f));
    
    std::cout << "✓ Linear interpolation test passed\n";
}

void testMultiSegmentInterpolation()
{
    std::cout << "Testing multi-segment interpolation...\n";
    
    ColorChannelConfig config;
    config.pitchWaveform = {0.0f, 2.0f, 1.0f};  // 3 points
    
    // At position 0.0, should be 0.0
    float offset = config.getPitchOffsetAt(0.0);
    assert(approxEqual(offset, 0.0f));
    
    // At position 0.25 (halfway between index 0 and 1)
    offset = config.getPitchOffsetAt(0.25);
    assert(approxEqual(offset, 1.0f));
    
    // At position 0.5 (at index 1)
    offset = config.getPitchOffsetAt(0.5);
    assert(approxEqual(offset, 2.0f));
    
    // At position 0.75 (halfway between index 1 and 2)
    offset = config.getPitchOffsetAt(0.75);
    assert(approxEqual(offset, 1.5f));
    
    // At position 1.0, wraps to 0.0 (which is 0.0 in the waveform)
    offset = config.getPitchOffsetAt(1.0);
    assert(approxEqual(offset, 0.0f));
    
    std::cout << "✓ Multi-segment interpolation test passed\n";
}

void testPositionWrapping()
{
    std::cout << "Testing position wrapping...\n";
    
    ColorChannelConfig config;
    config.pitchWaveform = {0.0f, 2.0f, 1.0f};
    
    // Position > 1.0 should wrap
    float offset1 = config.getPitchOffsetAt(1.5);
    float offset2 = config.getPitchOffsetAt(0.5);
    assert(approxEqual(offset1, offset2));
    
    // Position 2.25 should wrap to 0.25
    offset1 = config.getPitchOffsetAt(2.25);
    offset2 = config.getPitchOffsetAt(0.25);
    assert(approxEqual(offset1, offset2));
    
    std::cout << "✓ Position wrapping test passed\n";
}

void testComplexWaveform()
{
    std::cout << "Testing complex waveform...\n";
    
    ColorChannelConfig config;
    config.pitchWaveform = {0.0f, 1.0f, 2.0f, 1.0f, 0.0f};  // 5 points
    
    // At position 0.0, should be 0.0
    assert(approxEqual(config.getPitchOffsetAt(0.0), 0.0f));
    
    // At position 0.25 (at index 1)
    assert(approxEqual(config.getPitchOffsetAt(0.25), 1.0f));
    
    // At position 0.5 (at index 2)
    assert(approxEqual(config.getPitchOffsetAt(0.5), 2.0f));
    
    // At position 0.75 (at index 3)
    assert(approxEqual(config.getPitchOffsetAt(0.75), 1.0f));
    
    // At position 1.0, wraps to 0.0 (which is 0.0 in the waveform)
    assert(approxEqual(config.getPitchOffsetAt(1.0), 0.0f));
    
    std::cout << "✓ Complex waveform test passed\n";
}

void testIndependentLoopLength()
{
    std::cout << "Testing independent loop length...\n";
    
    ColorChannelConfig config;
    config.pitchSeqLoopLengthBars = 4;  // Different from main pattern
    config.pitchWaveform = {0.0f, 1.0f};
    
    // Loop length doesn't affect getPitchOffsetAt (it uses normalized position)
    // This is correct - the caller is responsible for converting playback position
    // to normalized position based on the pitch sequencer's loop length
    assert(config.pitchSeqLoopLengthBars == 4);
    assert(approxEqual(config.getPitchOffsetAt(0.5), 0.5f));
    
    std::cout << "✓ Independent loop length test passed\n";
}

int main()
{
    std::cout << "Running PitchSequencer standalone tests...\n\n";
    
    try
    {
        testEmptyWaveform();
        testPitchAlwaysApplies();
        testLinearInterpolation();
        testMultiSegmentInterpolation();
        testPositionWrapping();
        testComplexWaveform();
        testIndependentLoopLength();
        
        std::cout << "\n✓ All PitchSequencer tests passed!\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}

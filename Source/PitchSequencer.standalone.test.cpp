// Standalone test for PitchSequencer (no JUCE dependency)
#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <algorithm>

// Minimal PitchSequencer struct for testing
struct PitchSequencer {
    std::vector<float> waveform;
    int loopLengthBars;
    bool visible;
    
    PitchSequencer()
        : loopLengthBars(2)
        , visible(false)
    {}
    
    float getPitchOffsetAt(double normalizedPosition) const {
        if (waveform.empty() || !visible) {
            return 0.0f;
        }
        
        // Wrap position to [0, 1)
        normalizedPosition = normalizedPosition - std::floor(normalizedPosition);
        
        // Map to waveform index
        double indexFloat = normalizedPosition * (waveform.size() - 1);
        int index0 = static_cast<int>(std::floor(indexFloat));
        int index1 = std::min(index0 + 1, static_cast<int>(waveform.size() - 1));
        
        // Linear interpolation
        float t = static_cast<float>(indexFloat - index0);
        return waveform[index0] * (1.0f - t) + waveform[index1] * t;
    }
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
    
    PitchSequencer seq;
    seq.visible = true;
    
    assert(seq.getPitchOffsetAt(0.0) == 0.0f);
    assert(seq.getPitchOffsetAt(0.5) == 0.0f);
    assert(seq.getPitchOffsetAt(1.0) == 0.0f);
    
    std::cout << "✓ Empty waveform test passed\n";
}

void testHiddenSequencer()
{
    std::cout << "Testing hidden sequencer returns 0...\n";
    
    PitchSequencer seq;
    seq.waveform = {1.0f, 2.0f, 3.0f};
    seq.visible = false;
    
    assert(seq.getPitchOffsetAt(0.0) == 0.0f);
    assert(seq.getPitchOffsetAt(0.5) == 0.0f);
    assert(seq.getPitchOffsetAt(1.0) == 0.0f);
    
    std::cout << "✓ Hidden sequencer test passed\n";
}

void testLinearInterpolation()
{
    std::cout << "Testing linear interpolation...\n";
    
    PitchSequencer seq;
    seq.visible = true;
    seq.waveform = {0.0f, 4.0f};  // Simple case: 0 at start, 4 at end
    
    // At position 0.0, should be 0.0
    float offset = seq.getPitchOffsetAt(0.0);
    assert(approxEqual(offset, 0.0f));
    
    // At position 0.5, should be 2.0 (halfway between 0 and 4)
    offset = seq.getPitchOffsetAt(0.5);
    assert(approxEqual(offset, 2.0f));
    
    // At position 1.0, wraps to 0.0 (which is 0.0 in the waveform)
    // This is expected behavior - position 1.0 wraps to the start
    offset = seq.getPitchOffsetAt(1.0);
    assert(approxEqual(offset, 0.0f));
    
    std::cout << "✓ Linear interpolation test passed\n";
}

void testMultiSegmentInterpolation()
{
    std::cout << "Testing multi-segment interpolation...\n";
    
    PitchSequencer seq;
    seq.visible = true;
    seq.waveform = {0.0f, 2.0f, 1.0f};  // 3 points
    
    // At position 0.0, should be 0.0
    float offset = seq.getPitchOffsetAt(0.0);
    assert(approxEqual(offset, 0.0f));
    
    // At position 0.25 (halfway between index 0 and 1)
    offset = seq.getPitchOffsetAt(0.25);
    assert(approxEqual(offset, 1.0f));
    
    // At position 0.5 (at index 1)
    offset = seq.getPitchOffsetAt(0.5);
    assert(approxEqual(offset, 2.0f));
    
    // At position 0.75 (halfway between index 1 and 2)
    offset = seq.getPitchOffsetAt(0.75);
    assert(approxEqual(offset, 1.5f));
    
    // At position 1.0, wraps to 0.0 (which is 1.0 in the waveform)
    offset = seq.getPitchOffsetAt(1.0);
    assert(approxEqual(offset, 0.0f));
    
    std::cout << "✓ Multi-segment interpolation test passed\n";
}

void testPositionWrapping()
{
    std::cout << "Testing position wrapping...\n";
    
    PitchSequencer seq;
    seq.visible = true;
    seq.waveform = {0.0f, 2.0f, 1.0f};
    
    // Position > 1.0 should wrap
    float offset1 = seq.getPitchOffsetAt(1.5);
    float offset2 = seq.getPitchOffsetAt(0.5);
    assert(approxEqual(offset1, offset2));
    
    // Position 2.25 should wrap to 0.25
    offset1 = seq.getPitchOffsetAt(2.25);
    offset2 = seq.getPitchOffsetAt(0.25);
    assert(approxEqual(offset1, offset2));
    
    std::cout << "✓ Position wrapping test passed\n";
}

void testComplexWaveform()
{
    std::cout << "Testing complex waveform...\n";
    
    PitchSequencer seq;
    seq.visible = true;
    seq.waveform = {0.0f, 1.0f, 2.0f, 1.0f, 0.0f};  // 5 points
    
    // At position 0.0, should be 0.0
    assert(approxEqual(seq.getPitchOffsetAt(0.0), 0.0f));
    
    // At position 0.25 (at index 1)
    assert(approxEqual(seq.getPitchOffsetAt(0.25), 1.0f));
    
    // At position 0.5 (at index 2)
    assert(approxEqual(seq.getPitchOffsetAt(0.5), 2.0f));
    
    // At position 0.75 (at index 3)
    assert(approxEqual(seq.getPitchOffsetAt(0.75), 1.0f));
    
    // At position 1.0, wraps to 0.0 (which is 0.0 in the waveform)
    assert(approxEqual(seq.getPitchOffsetAt(1.0), 0.0f));
    
    std::cout << "✓ Complex waveform test passed\n";
}

void testIndependentLoopLength()
{
    std::cout << "Testing independent loop length...\n";
    
    PitchSequencer seq;
    seq.visible = true;
    seq.loopLengthBars = 4;  // Different from main pattern
    seq.waveform = {0.0f, 1.0f};
    
    // Loop length doesn't affect getPitchOffsetAt (it uses normalized position)
    // This is correct - the caller is responsible for converting playback position
    // to normalized position based on the pitch sequencer's loop length
    assert(seq.loopLengthBars == 4);
    assert(approxEqual(seq.getPitchOffsetAt(0.5), 0.5f));
    
    std::cout << "✓ Independent loop length test passed\n";
}

int main()
{
    std::cout << "Running PitchSequencer standalone tests...\n\n";
    
    try
    {
        testEmptyWaveform();
        testHiddenSequencer();
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

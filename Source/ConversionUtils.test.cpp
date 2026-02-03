#include "ConversionUtils.h"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace SquareBeats;

/**
 * Unit tests for ConversionUtils
 * These tests verify coordinate/time conversion and MIDI mapping functions.
 */

// Helper function for approximate float comparison
bool approxEqual(double a, double b, double epsilon = 0.0001)
{
    return std::abs(a - b) < epsilon;
}

bool approxEqual(float a, float b, float epsilon = 0.0001f)
{
    return std::abs(a - b) < epsilon;
}

//==============================================================================
// Time Conversion Tests

void testNormalizedToBeats()
{
    std::cout << "Testing normalizedToBeats()...\n";
    
    // 4/4 time signature, 2 bar loop
    TimeSignature timeSig(4, 4);
    int loopLength = 2;
    
    // 0.0 should be 0 beats
    assert(approxEqual(normalizedToBeats(0.0f, loopLength, timeSig), 0.0));
    
    // 1.0 should be 8 beats (2 bars × 4 beats/bar)
    assert(approxEqual(normalizedToBeats(1.0f, loopLength, timeSig), 8.0));
    
    // 0.5 should be 4 beats (halfway through)
    assert(approxEqual(normalizedToBeats(0.5f, loopLength, timeSig), 4.0));
    
    // 0.25 should be 2 beats (one bar)
    assert(approxEqual(normalizedToBeats(0.25f, loopLength, timeSig), 2.0));
    
    // 3/4 time signature, 1 bar loop
    TimeSignature timeSig34(3, 4);
    loopLength = 1;
    
    // 1.0 should be 3 beats (1 bar × 3 beats/bar)
    assert(approxEqual(normalizedToBeats(1.0f, loopLength, timeSig34), 3.0));
    
    // 0.5 should be 1.5 beats
    assert(approxEqual(normalizedToBeats(0.5f, loopLength, timeSig34), 1.5));
    
    // 6/8 time signature, 4 bar loop
    TimeSignature timeSig68(6, 8);
    loopLength = 4;
    
    // 6/8 = 6 × (4/8) = 3 beats per bar
    // 4 bars = 12 beats total
    assert(approxEqual(normalizedToBeats(1.0f, loopLength, timeSig68), 12.0));
    
    // 0.25 should be 3 beats (one bar)
    assert(approxEqual(normalizedToBeats(0.25f, loopLength, timeSig68), 3.0));
    
    std::cout << "✓ normalizedToBeats() test passed\n";
}

void testBeatsToNormalized()
{
    std::cout << "Testing beatsToNormalized()...\n";
    
    // 4/4 time signature, 2 bar loop
    TimeSignature timeSig(4, 4);
    int loopLength = 2;
    
    // 0 beats should be 0.0
    assert(approxEqual(beatsToNormalized(0.0, loopLength, timeSig), 0.0f));
    
    // 8 beats should be 1.0 (2 bars × 4 beats/bar)
    assert(approxEqual(beatsToNormalized(8.0, loopLength, timeSig), 1.0f));
    
    // 4 beats should be 0.5 (halfway through)
    assert(approxEqual(beatsToNormalized(4.0, loopLength, timeSig), 0.5f));
    
    // 2 beats should be 0.25 (one bar)
    assert(approxEqual(beatsToNormalized(2.0, loopLength, timeSig), 0.25f));
    
    // 3/4 time signature, 1 bar loop
    TimeSignature timeSig34(3, 4);
    loopLength = 1;
    
    // 3 beats should be 1.0 (1 bar × 3 beats/bar)
    assert(approxEqual(beatsToNormalized(3.0, loopLength, timeSig34), 1.0f));
    
    // 1.5 beats should be 0.5
    assert(approxEqual(beatsToNormalized(1.5, loopLength, timeSig34), 0.5f));
    
    std::cout << "✓ beatsToNormalized() test passed\n";
}

void testTimeConversionRoundTrip()
{
    std::cout << "Testing time conversion round-trip...\n";
    
    TimeSignature timeSig(4, 4);
    int loopLength = 2;
    
    // normalized -> beats -> normalized
    float original = 0.75f;
    double beats = normalizedToBeats(original, loopLength, timeSig);
    float result = beatsToNormalized(beats, loopLength, timeSig);
    assert(approxEqual(result, original));
    
    // beats -> normalized -> beats
    double originalBeats = 5.5;
    float normalized = beatsToNormalized(originalBeats, loopLength, timeSig);
    double resultBeats = normalizedToBeats(normalized, loopLength, timeSig);
    assert(approxEqual(resultBeats, originalBeats));
    
    std::cout << "✓ Time conversion round-trip test passed\n";
}

//==============================================================================
// MIDI Mapping Tests

void testMapVerticalPositionToNote()
{
    std::cout << "Testing mapVerticalPositionToNote()...\n";
    
    int highNote = 84;  // C6
    int lowNote = 48;   // C3
    float pitchOffset = 0.0f;
    
    // Top position (0.0) should map to high note
    assert(mapVerticalPositionToNote(0.0f, highNote, lowNote, pitchOffset) == 84);
    
    // Bottom position (1.0) should map to low note
    assert(mapVerticalPositionToNote(1.0f, highNote, lowNote, pitchOffset) == 48);
    
    // Middle position (0.5) should map to middle note
    int middleNote = (84 + 48) / 2;  // 66
    assert(mapVerticalPositionToNote(0.5f, highNote, lowNote, pitchOffset) == 66);
    
    // Test with positive pitch offset
    highNote = 60;  // C4
    lowNote = 48;   // C3
    pitchOffset = 12.0f;  // +1 octave
    
    // Top position with +12 offset should be 72 (C5)
    assert(mapVerticalPositionToNote(0.0f, highNote, lowNote, pitchOffset) == 72);
    
    // Bottom position with +12 offset should be 60 (C4)
    assert(mapVerticalPositionToNote(1.0f, highNote, lowNote, pitchOffset) == 60);
    
    // Test with negative pitch offset
    highNote = 72;  // C5
    lowNote = 60;   // C4
    pitchOffset = -12.0f;  // -1 octave
    
    // Top position with -12 offset should be 60 (C4)
    assert(mapVerticalPositionToNote(0.0f, highNote, lowNote, pitchOffset) == 60);
    
    // Bottom position with -12 offset should be 48 (C3)
    assert(mapVerticalPositionToNote(1.0f, highNote, lowNote, pitchOffset) == 48);
    
    // Test clamping to valid MIDI range (0-127)
    highNote = 120;
    lowNote = 100;
    
    // Positive offset that would exceed 127
    pitchOffset = 20.0f;
    assert(mapVerticalPositionToNote(0.0f, highNote, lowNote, pitchOffset) == 127);
    
    // Negative offset that would go below 0
    pitchOffset = -110.0f;
    assert(mapVerticalPositionToNote(1.0f, highNote, lowNote, pitchOffset) == 0);
    
    // Test fractional pitch offset rounding
    highNote = 60;
    lowNote = 60;
    
    // +0.4 semitones should round to 0 (60)
    assert(mapVerticalPositionToNote(0.0f, highNote, lowNote, 0.4f) == 60);
    
    // +0.5 semitones should round to 1 (61)
    assert(mapVerticalPositionToNote(0.0f, highNote, lowNote, 0.5f) == 61);
    
    // +0.6 semitones should round to 1 (61)
    assert(mapVerticalPositionToNote(0.0f, highNote, lowNote, 0.6f) == 61);
    
    std::cout << "✓ mapVerticalPositionToNote() test passed\n";
}

void testMapHeightToVelocity()
{
    std::cout << "Testing mapHeightToVelocity()...\n";
    
    // Minimum height maps to velocity 1
    assert(mapHeightToVelocity(0.0f) == 1);
    
    // Maximum height maps to velocity 127
    assert(mapHeightToVelocity(1.0f) == 127);
    
    // Middle height maps to middle velocity (around 64)
    int velocity = mapHeightToVelocity(0.5f);
    assert(velocity >= 63 && velocity <= 65);
    
    // Quarter height maps correctly (around 32-33)
    velocity = mapHeightToVelocity(0.25f);
    assert(velocity >= 31 && velocity <= 34);
    
    // Three-quarter height maps correctly (around 95-96)
    velocity = mapHeightToVelocity(0.75f);
    assert(velocity >= 94 && velocity <= 97);
    
    // Very small height still produces velocity >= 1
    assert(mapHeightToVelocity(0.001f) >= 1);
    assert(mapHeightToVelocity(0.01f) >= 1);
    
    // Height slightly over 1.0 clamps to 127
    assert(mapHeightToVelocity(1.1f) == 127);
    
    std::cout << "✓ mapHeightToVelocity() test passed\n";
}

void testMIDIMappingEdgeCases()
{
    std::cout << "Testing MIDI mapping edge cases...\n";
    
    // Inverted pitch range (high < low)
    int highNote = 48;  // C3
    int lowNote = 84;   // C6
    
    // Top position should map to 48
    assert(mapVerticalPositionToNote(0.0f, highNote, lowNote, 0.0f) == 48);
    
    // Bottom position should map to 84
    assert(mapVerticalPositionToNote(1.0f, highNote, lowNote, 0.0f) == 84);
    
    // Same high and low note
    int note = 60;  // C4
    
    // All positions should map to the same note (with offset)
    assert(mapVerticalPositionToNote(0.0f, note, note, 0.0f) == 60);
    assert(mapVerticalPositionToNote(0.5f, note, note, 0.0f) == 60);
    assert(mapVerticalPositionToNote(1.0f, note, note, 0.0f) == 60);
    
    // With offset
    assert(mapVerticalPositionToNote(0.5f, note, note, 5.0f) == 65);
    
    std::cout << "✓ MIDI mapping edge cases test passed\n";
}

//==============================================================================
// Main test runner

int main()
{
    std::cout << "Running ConversionUtils unit tests...\n\n";
    
    try
    {
        testNormalizedToBeats();
        testBeatsToNormalized();
        testTimeConversionRoundTrip();
        testMapVerticalPositionToNote();
        testMapHeightToVelocity();
        testMIDIMappingEdgeCases();
        
        std::cout << "\n✓ All ConversionUtils tests passed!\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}


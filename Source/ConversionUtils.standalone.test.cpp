// Standalone test for ConversionUtils (no JUCE dependency)
#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>

// Minimal TimeSignature struct for testing
struct TimeSignature {
    int numerator;
    int denominator;
    
    TimeSignature() : numerator(4), denominator(4) {}
    TimeSignature(int num, int denom) : numerator(num), denominator(denom) {}
    
    double getBeatsPerBar() const {
        return numerator * (4.0 / denominator);
    }
};

// Conversion functions (copied from ConversionUtils.h)
inline double normalizedToBeats(float normalized, int loopLengthBars, const TimeSignature& timeSig)
{
    double beatsPerBar = timeSig.getBeatsPerBar();
    double totalBeats = loopLengthBars * beatsPerBar;
    return normalized * totalBeats;
}

inline float beatsToNormalized(double beats, int loopLengthBars, const TimeSignature& timeSig)
{
    double beatsPerBar = timeSig.getBeatsPerBar();
    double totalBeats = loopLengthBars * beatsPerBar;
    return static_cast<float>(beats / totalBeats);
}

inline int mapVerticalPositionToNote(float normalizedY, int highNote, int lowNote, float pitchOffset)
{
    float noteFloat = highNote + (lowNote - highNote) * normalizedY + pitchOffset;
    int note = static_cast<int>(std::round(noteFloat));
    return std::clamp(note, 0, 127);
}

inline int mapHeightToVelocity(float normalizedHeight)
{
    int velocity = static_cast<int>(std::round(1.0f + normalizedHeight * 126.0f));
    return std::clamp(velocity, 1, 127);
}

// Helper for approximate comparison
bool approxEqual(double a, double b, double epsilon = 0.0001)
{
    return std::abs(a - b) < epsilon;
}

bool approxEqual(float a, float b, float epsilon = 0.0001f)
{
    return std::abs(a - b) < epsilon;
}

// Tests
void testNormalizedToBeats()
{
    std::cout << "Testing normalizedToBeats()...\n";
    
    TimeSignature timeSig(4, 4);
    int loopLength = 2;
    
    assert(approxEqual(normalizedToBeats(0.0f, loopLength, timeSig), 0.0));
    assert(approxEqual(normalizedToBeats(1.0f, loopLength, timeSig), 8.0));
    assert(approxEqual(normalizedToBeats(0.5f, loopLength, timeSig), 4.0));
    assert(approxEqual(normalizedToBeats(0.25f, loopLength, timeSig), 2.0));
    
    TimeSignature timeSig34(3, 4);
    loopLength = 1;
    assert(approxEqual(normalizedToBeats(1.0f, loopLength, timeSig34), 3.0));
    assert(approxEqual(normalizedToBeats(0.5f, loopLength, timeSig34), 1.5));
    
    std::cout << "✓ normalizedToBeats() test passed\n";
}

void testBeatsToNormalized()
{
    std::cout << "Testing beatsToNormalized()...\n";
    
    TimeSignature timeSig(4, 4);
    int loopLength = 2;
    
    assert(approxEqual(beatsToNormalized(0.0, loopLength, timeSig), 0.0f));
    assert(approxEqual(beatsToNormalized(8.0, loopLength, timeSig), 1.0f));
    assert(approxEqual(beatsToNormalized(4.0, loopLength, timeSig), 0.5f));
    assert(approxEqual(beatsToNormalized(2.0, loopLength, timeSig), 0.25f));
    
    std::cout << "✓ beatsToNormalized() test passed\n";
}

void testTimeConversionRoundTrip()
{
    std::cout << "Testing time conversion round-trip...\n";
    
    TimeSignature timeSig(4, 4);
    int loopLength = 2;
    
    float original = 0.75f;
    double beats = normalizedToBeats(original, loopLength, timeSig);
    float result = beatsToNormalized(beats, loopLength, timeSig);
    assert(approxEqual(result, original));
    
    double originalBeats = 5.5;
    float normalized = beatsToNormalized(originalBeats, loopLength, timeSig);
    double resultBeats = normalizedToBeats(normalized, loopLength, timeSig);
    assert(approxEqual(resultBeats, originalBeats));
    
    std::cout << "✓ Time conversion round-trip test passed\n";
}

void testMapVerticalPositionToNote()
{
    std::cout << "Testing mapVerticalPositionToNote()...\n";
    
    int highNote = 84;
    int lowNote = 48;
    float pitchOffset = 0.0f;
    
    assert(mapVerticalPositionToNote(0.0f, highNote, lowNote, pitchOffset) == 84);
    assert(mapVerticalPositionToNote(1.0f, highNote, lowNote, pitchOffset) == 48);
    assert(mapVerticalPositionToNote(0.5f, highNote, lowNote, pitchOffset) == 66);
    
    highNote = 60;
    lowNote = 48;
    pitchOffset = 12.0f;
    assert(mapVerticalPositionToNote(0.0f, highNote, lowNote, pitchOffset) == 72);
    assert(mapVerticalPositionToNote(1.0f, highNote, lowNote, pitchOffset) == 60);
    
    highNote = 120;
    lowNote = 100;
    pitchOffset = 20.0f;
    assert(mapVerticalPositionToNote(0.0f, highNote, lowNote, pitchOffset) == 127);
    
    pitchOffset = -110.0f;
    assert(mapVerticalPositionToNote(1.0f, highNote, lowNote, pitchOffset) == 0);
    
    std::cout << "✓ mapVerticalPositionToNote() test passed\n";
}

void testMapHeightToVelocity()
{
    std::cout << "Testing mapHeightToVelocity()...\n";
    
    assert(mapHeightToVelocity(0.0f) == 1);
    assert(mapHeightToVelocity(1.0f) == 127);
    
    int velocity = mapHeightToVelocity(0.5f);
    assert(velocity >= 63 && velocity <= 65);
    
    velocity = mapHeightToVelocity(0.25f);
    assert(velocity >= 31 && velocity <= 34);
    
    assert(mapHeightToVelocity(0.001f) >= 1);
    assert(mapHeightToVelocity(1.1f) == 127);
    
    std::cout << "✓ mapHeightToVelocity() test passed\n";
}

int main()
{
    std::cout << "Running ConversionUtils standalone tests...\n\n";
    
    try
    {
        testNormalizedToBeats();
        testBeatsToNormalized();
        testTimeConversionRoundTrip();
        testMapVerticalPositionToNote();
        testMapHeightToVelocity();
        
        std::cout << "\n✓ All ConversionUtils tests passed!\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}

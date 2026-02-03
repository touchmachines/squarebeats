// Standalone test for MIDIGenerator (minimal JUCE dependency)
#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <cstdint>

// Minimal data structures for testing
struct TimeSignature {
    int numerator;
    int denominator;
    
    TimeSignature() : numerator(4), denominator(4) {}
    TimeSignature(int num, int denom) : numerator(num), denominator(denom) {}
    
    double getBeatsPerBar() const {
        return numerator * (4.0 / denominator);
    }
};

enum QuantizationValue {
    Q_1_32,
    Q_1_16,
    Q_1_8,
    Q_1_4,
    Q_1_2,
    Q_1_BAR
};

struct ColorChannelConfig {
    int midiChannel;
    int highNote;
    int lowNote;
    QuantizationValue quantize;
    
    ColorChannelConfig()
        : midiChannel(1), highNote(84), lowNote(48), quantize(Q_1_16)
    {}
};

struct Square {
    float leftEdge;
    float width;
    float topEdge;
    float height;
    int colorChannelId;
    uint32_t uniqueId;
    
    Square(float left, float top, float w, float h, int colorId, uint32_t id)
        : leftEdge(left), width(w), topEdge(top), height(h)
        , colorChannelId(colorId), uniqueId(id)
    {}
    
    float getCenterY() const { return topEdge + height * 0.5f; }
};

// Conversion utilities
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

// MIDIGenerator functions (copied from implementation)
namespace MIDIGenerator {

int calculateMidiNote(const Square& square, 
                     const ColorChannelConfig& config,
                     float pitchOffset)
{
    float normalizedY = square.getCenterY();
    return mapVerticalPositionToNote(normalizedY, config.highNote, config.lowNote, pitchOffset);
}

int calculateVelocity(const Square& square)
{
    return mapHeightToVelocity(square.height);
}

double applyQuantization(double timeBeats, 
                        QuantizationValue quantize,
                        const TimeSignature& timeSig)
{
    double beatsPerBar = timeSig.getBeatsPerBar();
    double quantizeInterval;
    
    switch (quantize) {
        case Q_1_32:
            quantizeInterval = beatsPerBar / 32.0;
            break;
        case Q_1_16:
            quantizeInterval = beatsPerBar / 16.0;
            break;
        case Q_1_8:
            quantizeInterval = beatsPerBar / 8.0;
            break;
        case Q_1_4:
            quantizeInterval = beatsPerBar / 4.0;
            break;
        case Q_1_2:
            quantizeInterval = beatsPerBar / 2.0;
            break;
        case Q_1_BAR:
            quantizeInterval = beatsPerBar;
            break;
        default:
            quantizeInterval = beatsPerBar / 16.0;
            break;
    }
    
    return std::round(timeBeats / quantizeInterval) * quantizeInterval;
}

} // namespace MIDIGenerator

// Helper for approximate comparison
bool approxEqual(double a, double b, double epsilon = 0.0001)
{
    return std::abs(a - b) < epsilon;
}

// Tests
void testCalculateMidiNote()
{
    std::cout << "Testing calculateMidiNote()...\n";
    
    ColorChannelConfig config;
    config.highNote = 84;  // C6
    config.lowNote = 48;   // C3
    
    // Test square at top (centerY = 0.05, should map close to high note)
    Square topSquare(0.0f, 0.0f, 0.1f, 0.1f, 0, 1);
    int topNote = MIDIGenerator::calculateMidiNote(topSquare, config, 0.0f);
    // centerY = 0.05, note = 84 + (48-84)*0.05 = 84 - 1.8 = 82
    assert(topNote == 82);
    std::cout << "  Top position (centerY=0.05): " << topNote << " (expected 82)\n";
    
    // Test square exactly at top edge (height 0)
    Square exactTopSquare(0.0f, 0.0f, 0.1f, 0.0f, 0, 6);
    int exactTopNote = MIDIGenerator::calculateMidiNote(exactTopSquare, config, 0.0f);
    assert(exactTopNote == 84);
    std::cout << "  Exact top position (centerY=0.0): " << exactTopNote << " (expected 84)\n";
    
    // Test square at bottom (centerY = 0.95, should map close to low note)
    Square bottomSquare(0.0f, 0.9f, 0.1f, 0.1f, 0, 2);
    int bottomNote = MIDIGenerator::calculateMidiNote(bottomSquare, config, 0.0f);
    // centerY = 0.95, note = 84 + (48-84)*0.95 = 84 - 34.2 = 50
    assert(bottomNote == 50);
    std::cout << "  Bottom position (centerY=0.95): " << bottomNote << " (expected 50)\n";
    
    // Test square exactly at bottom edge
    Square exactBottomSquare(0.0f, 1.0f, 0.1f, 0.0f, 0, 7);
    int exactBottomNote = MIDIGenerator::calculateMidiNote(exactBottomSquare, config, 0.0f);
    assert(exactBottomNote == 48);
    std::cout << "  Exact bottom position (centerY=1.0): " << exactBottomNote << " (expected 48)\n";
    
    // Test square in middle (should map to middle note)
    Square midSquare(0.0f, 0.45f, 0.1f, 0.1f, 0, 3);
    int midNote = MIDIGenerator::calculateMidiNote(midSquare, config, 0.0f);
    // Center Y = 0.45 + 0.05 = 0.5, should map to (84 + 48) / 2 = 66
    assert(midNote == 66);
    std::cout << "  Middle position: " << midNote << " (expected 66)\n";
    
    // Test with pitch offset
    Square offsetSquare(0.0f, 0.0f, 0.1f, 0.0f, 0, 4);
    int offsetNote = MIDIGenerator::calculateMidiNote(offsetSquare, config, 12.0f);
    assert(offsetNote == 96);  // 84 + 12
    std::cout << "  With +12 offset (centerY=0.0): " << offsetNote << " (expected 96)\n";
    
    // Test clamping to valid MIDI range (high)
    Square highOffsetSquare(0.0f, 0.0f, 0.1f, 0.0f, 0, 8);
    int clampedHighNote = MIDIGenerator::calculateMidiNote(highOffsetSquare, config, 50.0f);
    assert(clampedHighNote == 127);
    std::cout << "  Clamped high: " << clampedHighNote << " (expected 127)\n";
    
    // Test clamping to valid MIDI range (low)
    Square lowSquare(0.0f, 1.0f, 0.1f, 0.0f, 0, 5);
    int clampedLowNote = MIDIGenerator::calculateMidiNote(lowSquare, config, -50.0f);
    assert(clampedLowNote == 0);
    std::cout << "  Clamped low: " << clampedLowNote << " (expected 0)\n";
    
    std::cout << "✓ calculateMidiNote() test passed\n";
}

void testCalculateVelocity()
{
    std::cout << "Testing calculateVelocity()...\n";
    
    // Test minimum height (should give velocity 1)
    Square minSquare(0.0f, 0.0f, 0.1f, 0.0f, 0, 1);
    int minVel = MIDIGenerator::calculateVelocity(minSquare);
    assert(minVel == 1);
    std::cout << "  Min height: " << minVel << " (expected 1)\n";
    
    // Test maximum height (should give velocity 127)
    Square maxSquare(0.0f, 0.0f, 0.1f, 1.0f, 0, 2);
    int maxVel = MIDIGenerator::calculateVelocity(maxSquare);
    assert(maxVel == 127);
    std::cout << "  Max height: " << maxVel << " (expected 127)\n";
    
    // Test middle height (should give velocity around 64)
    Square midSquare(0.0f, 0.0f, 0.1f, 0.5f, 0, 3);
    int midVel = MIDIGenerator::calculateVelocity(midSquare);
    assert(midVel >= 63 && midVel <= 65);  // Allow for rounding
    std::cout << "  Mid height: " << midVel << " (expected ~64)\n";
    
    std::cout << "✓ calculateVelocity() test passed\n";
}

void testApplyQuantization()
{
    std::cout << "Testing applyQuantization()...\n";
    
    TimeSignature timeSig(4, 4);  // 4/4 time, 4 beats per bar
    
    // Test 1/16 note quantization
    double time1 = 0.3;  // Should quantize to 0.25 (1/16 of 4 beats = 0.25)
    double quantized1 = MIDIGenerator::applyQuantization(time1, Q_1_16, timeSig);
    assert(approxEqual(quantized1, 0.25));
    std::cout << "  1/16 quantize 0.3: " << quantized1 << " (expected 0.25)\n";
    
    // Test 1/8 note quantization
    double time2 = 0.6;  // Should quantize to 0.5 (1/8 of 4 beats = 0.5)
    double quantized2 = MIDIGenerator::applyQuantization(time2, Q_1_8, timeSig);
    assert(approxEqual(quantized2, 0.5));
    std::cout << "  1/8 quantize 0.6: " << quantized2 << " (expected 0.5)\n";
    
    // Test 1/4 note quantization
    double time3 = 1.3;  // Should quantize to 1.0 (1/4 of 4 beats = 1.0)
    double quantized3 = MIDIGenerator::applyQuantization(time3, Q_1_4, timeSig);
    assert(approxEqual(quantized3, 1.0));
    std::cout << "  1/4 quantize 1.3: " << quantized3 << " (expected 1.0)\n";
    
    // Test 1/2 note quantization
    double time4 = 2.3;  // Should quantize to 2.0 (1/2 of 4 beats = 2.0)
    double quantized4 = MIDIGenerator::applyQuantization(time4, Q_1_2, timeSig);
    assert(approxEqual(quantized4, 2.0));
    std::cout << "  1/2 quantize 2.3: " << quantized4 << " (expected 2.0)\n";
    
    // Test 1 bar quantization
    double time5 = 5.0;  // Should quantize to 4.0 (1 bar = 4 beats)
    double quantized5 = MIDIGenerator::applyQuantization(time5, Q_1_BAR, timeSig);
    assert(approxEqual(quantized5, 4.0));
    std::cout << "  1 bar quantize 5.0: " << quantized5 << " (expected 4.0)\n";
    
    // Test 1/32 note quantization
    double time6 = 0.15;  // Should quantize to 0.125 (1/32 of 4 beats = 0.125)
    double quantized6 = MIDIGenerator::applyQuantization(time6, Q_1_32, timeSig);
    assert(approxEqual(quantized6, 0.125));
    std::cout << "  1/32 quantize 0.15: " << quantized6 << " (expected 0.125)\n";
    
    // Test with different time signature (3/4)
    TimeSignature timeSig34(3, 4);  // 3/4 time, 3 beats per bar
    double time7 = 1.6;  // Should quantize to 1.5 (1/4 of 3 beats = 0.75)
    double quantized7 = MIDIGenerator::applyQuantization(time7, Q_1_4, timeSig34);
    assert(approxEqual(quantized7, 1.5));
    std::cout << "  3/4 time 1/4 quantize 1.6: " << quantized7 << " (expected 1.5)\n";
    
    std::cout << "✓ applyQuantization() test passed\n";
}

void testQuantizationEdgeCases()
{
    std::cout << "Testing quantization edge cases...\n";
    
    TimeSignature timeSig(4, 4);
    
    // Test exact quantization boundaries
    double exact = 1.0;
    double quantized = MIDIGenerator::applyQuantization(exact, Q_1_4, timeSig);
    assert(approxEqual(quantized, 1.0));
    std::cout << "  Exact boundary 1.0: " << quantized << " (expected 1.0)\n";
    
    // Test zero
    double zero = 0.0;
    double quantizedZero = MIDIGenerator::applyQuantization(zero, Q_1_16, timeSig);
    assert(approxEqual(quantizedZero, 0.0));
    std::cout << "  Zero: " << quantizedZero << " (expected 0.0)\n";
    
    // Test rounding up vs down
    double justBelow = 0.24;  // Should round down to 0.0 for 1/4 quantization
    double quantizedDown = MIDIGenerator::applyQuantization(justBelow, Q_1_4, timeSig);
    assert(approxEqual(quantizedDown, 0.0));
    std::cout << "  Just below 0.25: " << quantizedDown << " (expected 0.0)\n";
    
    double justAbove = 0.6;  // Should round up to 1.0 for 1/4 quantization
    double quantizedUp = MIDIGenerator::applyQuantization(justAbove, Q_1_4, timeSig);
    assert(approxEqual(quantizedUp, 1.0));
    std::cout << "  0.6 with 1/4 quantization: " << quantizedUp << " (expected 1.0)\n";
    
    std::cout << "✓ Quantization edge cases test passed\n";
}

int main()
{
    std::cout << "Running MIDIGenerator standalone tests...\n\n";
    
    try
    {
        testCalculateMidiNote();
        testCalculateVelocity();
        testApplyQuantization();
        testQuantizationEdgeCases();
        
        std::cout << "\n✓ All MIDIGenerator tests passed!\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}

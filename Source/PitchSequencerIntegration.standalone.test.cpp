// Standalone test for Pitch Sequencer Integration with MIDI Generation
#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <vector>

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

// Pitch sequencer editing mode (doesn't affect pitch modulation)
struct PitchSequencer {
    bool editingPitch;  // True when editing pitch sequence, false when editing squares
    
    PitchSequencer()
        : editingPitch(false)
    {}
};

// Per-color pitch waveform configuration
struct ColorChannelConfig {
    int midiChannel;
    int highNote;
    int lowNote;
    std::vector<float> pitchWaveform;
    int pitchSeqLoopLengthBars;
    
    ColorChannelConfig()
        : midiChannel(1), highNote(84), lowNote(48), pitchSeqLoopLengthBars(2)
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

struct Square {
    float leftEdge;
    float width;
    float topEdge;
    float height;
    int colorChannelId;
    
    Square(float left, float top, float w, float h, int colorId)
        : leftEdge(left), width(w), topEdge(top), height(h), colorChannelId(colorId)
    {}
    
    float getCenterY() const { return topEdge + height * 0.5f; }
};

// MIDI note calculation with pitch offset
int calculateMidiNote(const Square& square, const ColorChannelConfig& config, float pitchOffset)
{
    float normalizedY = square.getCenterY();
    float noteFloat = config.highNote + (config.lowNote - config.highNote) * normalizedY + pitchOffset;
    int note = static_cast<int>(std::round(noteFloat));
    return std::clamp(note, 0, 127);
}

// Helper for approximate comparison
bool approxEqual(float a, float b, float epsilon = 0.01f)
{
    return std::abs(a - b) < epsilon;
}

// Tests
void testPitchOffsetAppliedToNote()
{
    std::cout << "Testing pitch offset applied to MIDI note...\n";
    
    Square square(0.0f, 0.5f, 0.25f, 0.5f, 0);
    ColorChannelConfig config;
    config.highNote = 60;  // C4
    config.lowNote = 48;   // C3
    
    // Without pitch offset
    int noteWithoutOffset = calculateMidiNote(square, config, 0.0f);
    // Square center Y is 0.75, so note should be 60 + (48-60)*0.75 = 60 - 9 = 51
    assert(noteWithoutOffset == 51);
    
    // With +12 semitone offset (one octave up)
    int noteWithOffset = calculateMidiNote(square, config, 12.0f);
    assert(noteWithOffset == 63);
    
    // With -12 semitone offset (one octave down)
    noteWithOffset = calculateMidiNote(square, config, -12.0f);
    assert(noteWithOffset == 39);
    
    std::cout << "✓ Pitch offset applied to MIDI note test passed\n";
}

void testPitchAlwaysApplies()
{
    std::cout << "Testing pitch modulation always applies (regardless of editing mode)...\n";
    
    ColorChannelConfig config;
    config.highNote = 60;
    config.lowNote = 48;
    config.pitchWaveform = {0.0f, 12.0f};
    
    PitchSequencer pitchSeq;
    pitchSeq.editingPitch = false;  // Not in editing mode - but pitch still applies!
    
    Square square(0.0f, 0.5f, 0.25f, 0.5f, 0);
    
    // Get pitch offset (should NOT be 0 - pitch always applies)
    float pitchOffset = config.getPitchOffsetAt(0.5);
    assert(approxEqual(pitchOffset, 6.0f));  // Halfway between 0 and 12
    
    // Calculate note with offset
    int noteWithOffset = calculateMidiNote(square, config, pitchOffset);
    int noteWithoutOffset = calculateMidiNote(square, config, 0.0f);
    assert(noteWithOffset == noteWithoutOffset + 6);
    
    std::cout << "✓ Pitch always applies test passed\n";
}

void testPitchWaveformAppliesOffset()
{
    std::cout << "Testing pitch waveform applies offset...\n";
    
    ColorChannelConfig config;
    config.highNote = 60;
    config.lowNote = 48;
    config.pitchWaveform = {0.0f, 12.0f};  // 0 at start, 12 at end
    
    Square square(0.0f, 0.5f, 0.25f, 0.5f, 0);
    
    // At position 0.0, offset should be 0
    float offset1 = config.getPitchOffsetAt(0.0);
    assert(approxEqual(offset1, 0.0f));
    int note1 = calculateMidiNote(square, config, offset1);
    
    // At position 0.5, offset should be 6 (halfway between 0 and 12)
    float offset2 = config.getPitchOffsetAt(0.5);
    assert(approxEqual(offset2, 6.0f));
    int note2 = calculateMidiNote(square, config, offset2);
    
    // Note should be 6 semitones higher
    assert(note2 == note1 + 6);
    
    std::cout << "✓ Pitch waveform applies offset test passed\n";
}

void testIndependentLoopLengthConversion()
{
    std::cout << "Testing independent loop length conversion...\n";
    
    // Main pattern: 2 bars at 4/4 = 8 beats
    int mainLoopBars = 2;
    TimeSignature timeSig(4, 4);
    double mainLoopBeats = mainLoopBars * timeSig.getBeatsPerBar();  // 8 beats
    (void)mainLoopBeats;  // Unused in this test
    
    // Pitch sequencer: 4 bars = 16 beats (per-color now)
    ColorChannelConfig config;
    config.pitchSeqLoopLengthBars = 4;
    config.pitchWaveform = {0.0f, 12.0f};
    
    // Simulate playback at beat 4 (halfway through main loop)
    double currentPositionBeats = 4.0;
    
    // Convert to pitch sequencer's normalized position
    double pitchSeqLoopBeats = config.pitchSeqLoopLengthBars * timeSig.getBeatsPerBar();  // 16 beats
    double normalizedPitchSeqPos = std::fmod(currentPositionBeats, pitchSeqLoopBeats) / pitchSeqLoopBeats;
    
    // At beat 4 in a 16-beat pitch sequencer loop, normalized position is 4/16 = 0.25
    assert(approxEqual(normalizedPitchSeqPos, 0.25f));
    
    // Pitch offset at 0.25 should be 3.0 (25% of the way from 0 to 12)
    float pitchOffset = config.getPitchOffsetAt(normalizedPitchSeqPos);
    assert(approxEqual(pitchOffset, 3.0f));
    
    std::cout << "✓ Independent loop length conversion test passed\n";
}

void testPitchOffsetClamping()
{
    std::cout << "Testing pitch offset clamping to MIDI range...\n";
    
    Square square(0.0f, 0.0f, 0.25f, 0.5f, 0);  // Top of plane
    ColorChannelConfig config;
    config.highNote = 120;  // Near top of MIDI range
    config.lowNote = 100;
    
    // With large positive offset, should clamp to 127
    int note = calculateMidiNote(square, config, 50.0f);
    assert(note == 127);
    
    // With large negative offset, should clamp to 0
    square.topEdge = 1.0f;  // Bottom of plane
    note = calculateMidiNote(square, config, -150.0f);
    assert(note == 0);
    
    std::cout << "✓ Pitch offset clamping test passed\n";
}

int main()
{
    std::cout << "Running Pitch Sequencer Integration Tests...\n\n";
    
    try
    {
        testPitchOffsetAppliedToNote();
        testPitchAlwaysApplies();
        testPitchWaveformAppliesOffset();
        testIndependentLoopLengthConversion();
        testPitchOffsetClamping();
        
        std::cout << "\n✓ All pitch sequencer integration tests passed!\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}

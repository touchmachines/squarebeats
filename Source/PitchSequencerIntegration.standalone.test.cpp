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

struct ColorChannelConfig {
    int midiChannel;
    int highNote;
    int lowNote;
    
    ColorChannelConfig()
        : midiChannel(1), highNote(84), lowNote(48)
    {}
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

void testHiddenSequencerZeroOffset()
{
    std::cout << "Testing hidden sequencer returns zero offset...\n";
    
    PitchSequencer pitchSeq;
    pitchSeq.waveform = {0.0f, 12.0f};
    pitchSeq.visible = false;  // Hidden
    
    Square square(0.0f, 0.5f, 0.25f, 0.5f, 0);
    ColorChannelConfig config;
    config.highNote = 60;
    config.lowNote = 48;
    
    // Get pitch offset (should be 0 because sequencer is hidden)
    float pitchOffset = pitchSeq.getPitchOffsetAt(0.5);
    assert(pitchOffset == 0.0f);
    
    // Calculate note with zero offset
    int note = calculateMidiNote(square, config, pitchOffset);
    int noteWithoutSequencer = calculateMidiNote(square, config, 0.0f);
    assert(note == noteWithoutSequencer);
    
    std::cout << "✓ Hidden sequencer zero offset test passed\n";
}

void testVisibleSequencerAppliesOffset()
{
    std::cout << "Testing visible sequencer applies offset...\n";
    
    PitchSequencer pitchSeq;
    pitchSeq.waveform = {0.0f, 12.0f};  // 0 at start, 12 at end
    pitchSeq.visible = true;
    
    Square square(0.0f, 0.5f, 0.25f, 0.5f, 0);
    ColorChannelConfig config;
    config.highNote = 60;
    config.lowNote = 48;
    
    // At position 0.0, offset should be 0
    float offset1 = pitchSeq.getPitchOffsetAt(0.0);
    assert(approxEqual(offset1, 0.0f));
    int note1 = calculateMidiNote(square, config, offset1);
    
    // At position 0.5, offset should be 6 (halfway between 0 and 12)
    float offset2 = pitchSeq.getPitchOffsetAt(0.5);
    assert(approxEqual(offset2, 6.0f));
    int note2 = calculateMidiNote(square, config, offset2);
    
    // Note should be 6 semitones higher
    assert(note2 == note1 + 6);
    
    std::cout << "✓ Visible sequencer applies offset test passed\n";
}

void testIndependentLoopLengthConversion()
{
    std::cout << "Testing independent loop length conversion...\n";
    
    // Main pattern: 2 bars at 4/4 = 8 beats
    int mainLoopBars = 2;
    TimeSignature timeSig(4, 4);
    double mainLoopBeats = mainLoopBars * timeSig.getBeatsPerBar();  // 8 beats
    
    // Pitch sequencer: 4 bars = 16 beats
    PitchSequencer pitchSeq;
    pitchSeq.loopLengthBars = 4;
    pitchSeq.visible = true;
    pitchSeq.waveform = {0.0f, 12.0f};
    
    // Simulate playback at beat 4 (halfway through main loop)
    double currentPositionBeats = 4.0;
    
    // Convert to pitch sequencer's normalized position
    double pitchSeqLoopBeats = pitchSeq.loopLengthBars * timeSig.getBeatsPerBar();  // 16 beats
    double normalizedPitchSeqPos = std::fmod(currentPositionBeats, pitchSeqLoopBeats) / pitchSeqLoopBeats;
    
    // At beat 4 in a 16-beat pitch sequencer loop, normalized position is 4/16 = 0.25
    assert(approxEqual(normalizedPitchSeqPos, 0.25f));
    
    // Pitch offset at 0.25 should be 3.0 (25% of the way from 0 to 12)
    float pitchOffset = pitchSeq.getPitchOffsetAt(normalizedPitchSeqPos);
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
        testHiddenSequencerZeroOffset();
        testVisibleSequencerAppliesOffset();
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

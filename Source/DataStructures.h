#pragma once

#include <juce_graphics/juce_graphics.h>
#include <vector>
#include <cstdint>

namespace SquareBeats {

//==============================================================================
/**
 * Time signature configuration
 */
struct TimeSignature {
    int numerator;   // 1-16
    int denominator; // 1, 2, 4, 8, 16
    
    TimeSignature() : numerator(4), denominator(4) {}
    TimeSignature(int num, int denom) : numerator(num), denominator(denom) {}
    
    double getBeatsPerBar() const {
        return numerator * (4.0 / denominator);
    }
};

//==============================================================================
/**
 * Quantization values for MIDI event timing
 */
enum QuantizationValue {
    Q_1_32,  // 1/32 note
    Q_1_16,  // 1/16 note
    Q_1_8,   // 1/8 note
    Q_1_4,   // 1/4 note
    Q_1_2,   // 1/2 note
    Q_1_BAR  // 1 bar
};

//==============================================================================
/**
 * Root note for scale (0 = C, 1 = C#, etc.)
 */
enum RootNote {
    ROOT_C = 0,
    ROOT_C_SHARP,
    ROOT_D,
    ROOT_D_SHARP,
    ROOT_E,
    ROOT_F,
    ROOT_F_SHARP,
    ROOT_G,
    ROOT_G_SHARP,
    ROOT_A,
    ROOT_A_SHARP,
    ROOT_B
};

/**
 * Scale type definitions
 */
enum ScaleType {
    SCALE_CHROMATIC = 0,    // All 12 notes (no filtering)
    SCALE_MAJOR,            // Ionian
    SCALE_NATURAL_MINOR,    // Aeolian
    SCALE_HARMONIC_MINOR,
    SCALE_MELODIC_MINOR,
    SCALE_PENTATONIC_MAJOR,
    SCALE_PENTATONIC_MINOR,
    SCALE_BLUES,
    SCALE_DORIAN,
    SCALE_PHRYGIAN,
    SCALE_LYDIAN,
    SCALE_MIXOLYDIAN,
    SCALE_LOCRIAN,
    SCALE_WHOLE_TONE,
    SCALE_DIMINISHED_HW,    // Half-whole
    SCALE_DIMINISHED_WH,    // Whole-half
    NUM_SCALE_TYPES
};

/**
 * Scale configuration with root note and scale type
 */
struct ScaleConfig {
    RootNote rootNote;
    ScaleType scaleType;
    
    ScaleConfig() : rootNote(ROOT_C), scaleType(SCALE_CHROMATIC) {}
    ScaleConfig(RootNote root, ScaleType scale) : rootNote(root), scaleType(scale) {}
    
    /**
     * Get the intervals (semitones from root) for the current scale type
     * Returns a vector of semitone offsets within one octave (0-11)
     */
    static std::vector<int> getScaleIntervals(ScaleType scale) {
        switch (scale) {
            case SCALE_CHROMATIC:
                return {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
            case SCALE_MAJOR:
                return {0, 2, 4, 5, 7, 9, 11};  // W W H W W W H
            case SCALE_NATURAL_MINOR:
                return {0, 2, 3, 5, 7, 8, 10};  // W H W W H W W
            case SCALE_HARMONIC_MINOR:
                return {0, 2, 3, 5, 7, 8, 11};  // W H W W H 1.5 H
            case SCALE_MELODIC_MINOR:
                return {0, 2, 3, 5, 7, 9, 11};  // W H W W W W H
            case SCALE_PENTATONIC_MAJOR:
                return {0, 2, 4, 7, 9};
            case SCALE_PENTATONIC_MINOR:
                return {0, 3, 5, 7, 10};
            case SCALE_BLUES:
                return {0, 3, 5, 6, 7, 10};
            case SCALE_DORIAN:
                return {0, 2, 3, 5, 7, 9, 10};
            case SCALE_PHRYGIAN:
                return {0, 1, 3, 5, 7, 8, 10};
            case SCALE_LYDIAN:
                return {0, 2, 4, 6, 7, 9, 11};
            case SCALE_MIXOLYDIAN:
                return {0, 2, 4, 5, 7, 9, 10};
            case SCALE_LOCRIAN:
                return {0, 1, 3, 5, 6, 8, 10};
            case SCALE_WHOLE_TONE:
                return {0, 2, 4, 6, 8, 10};
            case SCALE_DIMINISHED_HW:
                return {0, 1, 3, 4, 6, 7, 9, 10};
            case SCALE_DIMINISHED_WH:
                return {0, 2, 3, 5, 6, 8, 9, 11};
            default:
                return {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        }
    }
    
    /**
     * Snap a MIDI note to the nearest note in the scale
     * @param midiNote Input MIDI note (0-127)
     * @return Snapped MIDI note (0-127)
     */
    int snapToScale(int midiNote) const {
        if (scaleType == SCALE_CHROMATIC) {
            return midiNote;  // No snapping needed
        }
        
        auto intervals = getScaleIntervals(scaleType);
        if (intervals.empty()) {
            return midiNote;
        }
        
        // Get the note's position relative to the root
        int noteInOctave = ((midiNote % 12) - static_cast<int>(rootNote) + 12) % 12;
        int octave = midiNote / 12;
        
        // Find the nearest scale degree
        int bestInterval = intervals[0];
        int minDistance = 12;
        
        for (int interval : intervals) {
            int distance = std::abs(noteInOctave - interval);
            // Also check wrapping around the octave
            int wrapDistance = 12 - distance;
            int actualDistance = std::min(distance, wrapDistance);
            
            if (actualDistance < minDistance) {
                minDistance = actualDistance;
                bestInterval = interval;
            }
        }
        
        // Calculate the snapped note
        int snappedNoteInOctave = (bestInterval + static_cast<int>(rootNote)) % 12;
        int snappedNote = octave * 12 + snappedNoteInOctave;
        
        // Handle octave boundary cases
        if (snappedNote < 0) snappedNote = 0;
        if (snappedNote > 127) snappedNote = 127;
        
        return snappedNote;
    }
    
    /**
     * Get display name for a root note
     */
    static const char* getRootNoteName(RootNote root) {
        static const char* names[] = {
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
        };
        return names[static_cast<int>(root) % 12];
    }
    
    /**
     * Get display name for a scale type
     */
    static const char* getScaleTypeName(ScaleType scale) {
        static const char* names[] = {
            "Chromatic",
            "Major",
            "Natural Minor",
            "Harmonic Minor",
            "Melodic Minor",
            "Pentatonic Maj",
            "Pentatonic Min",
            "Blues",
            "Dorian",
            "Phrygian",
            "Lydian",
            "Mixolydian",
            "Locrian",
            "Whole Tone",
            "Diminished H-W",
            "Diminished W-H"
        };
        if (scale >= 0 && scale < NUM_SCALE_TYPES) {
            return names[scale];
        }
        return "Unknown";
    }
};

//==============================================================================
/**
 * Color channel configuration
 * Each color channel has independent MIDI routing and pitch/quantization settings
 */
struct ColorChannelConfig {
    int midiChannel;           // 1-16
    int highNote;              // MIDI note number (0-127) for top of sequencing plane
    int lowNote;               // MIDI note number (0-127) for bottom of sequencing plane
    QuantizationValue quantize; // Quantization setting
    juce::Colour displayColor; // UI rendering color
    std::vector<float> pitchWaveform; // Per-color pitch sequencer waveform (semitones)
    int pitchSeqLoopLengthBars; // Per-color pitch sequencer loop length (1-32 bars)
    
    ColorChannelConfig()
        : midiChannel(1)
        , highNote(84)  // C6
        , lowNote(48)   // C3
        , quantize(Q_1_16)
        , displayColor(juce::Colours::red)
        , pitchSeqLoopLengthBars(2)
    {}
    
    /**
     * Get pitch offset at normalized position (0.0 to 1.0)
     * Uses linear interpolation between samples
     */
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

//==============================================================================
/**
 * Pitch sequencer global settings
 * The actual waveform data and loop length are stored per-color in ColorChannelConfig
 */
struct PitchSequencer {
    bool editingPitch;           // True when editing pitch sequence, false when editing squares
    
    PitchSequencer()
        : editingPitch(false)
    {}
};

//==============================================================================
/**
 * Play mode for sequencer playback direction
 */
enum PlayMode {
    PLAY_FORWARD = 0,    // --> Normal forward playback
    PLAY_BACKWARD,       // <-- Reverse playback
    PLAY_PENDULUM,       // <--> Bounce back and forth
    PLAY_PROBABILITY,    // --?> Random step jumps
    NUM_PLAY_MODES
};

/**
 * Play mode configuration with probability settings
 */
struct PlayModeConfig {
    PlayMode mode;
    float stepJumpSize;      // 0.0 to 1.0 (normalized, maps to 1-16 steps)
    float probability;       // 0.0 to 1.0 (chance of jumping vs normal step)
    bool pendulumForward;    // Internal state: current direction in pendulum mode
    
    PlayModeConfig()
        : mode(PLAY_FORWARD)
        , stepJumpSize(0.5f)
        , probability(0.5f)
        , pendulumForward(true)
    {}
    
    /**
     * Get display name for a play mode
     */
    static const char* getPlayModeName(PlayMode mode) {
        static const char* names[] = {
            "-->",      // Forward
            "<--",      // Backward
            "<-->",     // Pendulum
            "--?>"      // Probability
        };
        if (mode >= 0 && mode < NUM_PLAY_MODES) {
            return names[mode];
        }
        return "Unknown";
    }
    
    /**
     * Get the actual step jump size in steps (1-16)
     */
    int getStepJumpSteps() const {
        return 1 + static_cast<int>(stepJumpSize * 15.0f);  // Maps 0.0-1.0 to 1-16
    }
};

//==============================================================================
/**
 * A single segment in the scale sequence
 */
struct ScaleSequenceSegment {
    RootNote rootNote;
    ScaleType scaleType;
    int lengthBars;  // Duration in bars (1-16)
    
    ScaleSequenceSegment()
        : rootNote(ROOT_C)
        , scaleType(SCALE_MAJOR)
        , lengthBars(2)
    {}
    
    ScaleSequenceSegment(RootNote root, ScaleType scale, int bars)
        : rootNote(root)
        , scaleType(scale)
        , lengthBars(bars)
    {}
    
    ScaleConfig toScaleConfig() const {
        return ScaleConfig(rootNote, scaleType);
    }
};

/**
 * Scale sequencer configuration - chains multiple key/scale segments
 */
struct ScaleSequencerConfig {
    bool enabled;                              // Whether scale sequencing is active
    std::vector<ScaleSequenceSegment> segments; // The sequence of scale changes
    
    static constexpr int MAX_SEGMENTS = 16;    // Maximum number of segments allowed
    
    ScaleSequencerConfig()
        : enabled(false)
    {
        // Start with one default segment
        segments.push_back(ScaleSequenceSegment(ROOT_C, SCALE_MAJOR, 4));
    }
    
    /**
     * Get total length of the scale sequence in bars
     */
    int getTotalLengthBars() const {
        int total = 0;
        for (const auto& seg : segments) {
            total += seg.lengthBars;
        }
        return total;
    }
    
    /**
     * Get the scale config at a given position in bars (wraps around)
     * @param positionBars Position in bars from start
     * @return The ScaleConfig active at that position
     */
    ScaleConfig getScaleAtPosition(double positionBars) const {
        if (segments.empty()) {
            return ScaleConfig();  // Default C Chromatic
        }
        
        int totalBars = getTotalLengthBars();
        if (totalBars <= 0) {
            return segments[0].toScaleConfig();
        }
        
        // Wrap position to sequence length
        double wrappedPos = std::fmod(positionBars, static_cast<double>(totalBars));
        if (wrappedPos < 0) wrappedPos += totalBars;
        
        // Find which segment we're in
        double accumulated = 0.0;
        for (const auto& seg : segments) {
            accumulated += seg.lengthBars;
            if (wrappedPos < accumulated) {
                return seg.toScaleConfig();
            }
        }
        
        // Fallback to last segment
        return segments.back().toScaleConfig();
    }
    
    /**
     * Get the index of the segment at a given position in bars
     */
    int getSegmentIndexAtPosition(double positionBars) const {
        if (segments.empty()) return -1;
        
        int totalBars = getTotalLengthBars();
        if (totalBars <= 0) return 0;
        
        double wrappedPos = std::fmod(positionBars, static_cast<double>(totalBars));
        if (wrappedPos < 0) wrappedPos += totalBars;
        
        double accumulated = 0.0;
        for (size_t i = 0; i < segments.size(); ++i) {
            accumulated += segments[i].lengthBars;
            if (wrappedPos < accumulated) {
                return static_cast<int>(i);
            }
        }
        
        return static_cast<int>(segments.size() - 1);
    }
};

//==============================================================================
/**
 * Square represents a MIDI note event on the sequencing plane
 * All coordinates are normalized (0.0 to 1.0) relative to the sequencing plane
 */
struct Square {
    float leftEdge;      // Normalized time position (0.0 to 1.0 within loop)
    float width;         // Normalized duration (0.0 to 1.0)
    float topEdge;       // Normalized vertical position (0.0 = top)
    float height;        // Normalized vertical size (0.0 to 1.0)
    int colorChannelId;  // Index of assigned color channel (0-3)
    uint32_t uniqueId;   // Unique identifier for tracking and editing
    
    Square()
        : leftEdge(0.0f)
        , width(0.0f)
        , topEdge(0.0f)
        , height(0.0f)
        , colorChannelId(0)
        , uniqueId(0)
    {}
    
    Square(float left, float top, float w, float h, int colorId, uint32_t id)
        : leftEdge(left)
        , width(w)
        , topEdge(top)
        , height(h)
        , colorChannelId(colorId)
        , uniqueId(id)
    {}
    
    // Helper methods
    float getRightEdge() const { return leftEdge + width; }
    float getBottomEdge() const { return topEdge + height; }
    float getCenterY() const { return topEdge + height * 0.5f; }
};

} // namespace SquareBeats

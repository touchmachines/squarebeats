#pragma once

#include "DataStructures.h"
#include <algorithm>
#include <cmath>

namespace SquareBeats {

//==============================================================================
/**
 * Coordinate and Time Conversion Utilities
 * 
 * These functions convert between normalized coordinates (0.0 to 1.0) and
 * musical time in beats, as well as mapping spatial properties to MIDI values.
 */

//==============================================================================
// Time conversion functions

/**
 * Convert normalized time position to beats
 * @param normalized Normalized time (0.0 to 1.0 within loop)
 * @param loopLengthBars Loop length in bars (supports fractional bars for steps)
 * @param timeSig Time signature
 * @return Time position in beats
 */
inline double normalizedToBeats(float normalized, double loopLengthBars, const TimeSignature& timeSig)
{
    double beatsPerBar = timeSig.getBeatsPerBar();
    double totalBeats = loopLengthBars * beatsPerBar;
    return normalized * totalBeats;
}

/**
 * Convert beats to normalized time position
 * @param beats Time position in beats
 * @param loopLengthBars Loop length in bars (supports fractional bars for steps)
 * @param timeSig Time signature
 * @return Normalized time (0.0 to 1.0 within loop)
 */
inline float beatsToNormalized(double beats, double loopLengthBars, const TimeSignature& timeSig)
{
    double beatsPerBar = timeSig.getBeatsPerBar();
    double totalBeats = loopLengthBars * beatsPerBar;
    return static_cast<float>(beats / totalBeats);
}

//==============================================================================
// MIDI mapping functions

/**
 * Map vertical position to MIDI note number with pitch offset
 * @param normalizedY Normalized vertical position (0.0 = top, 1.0 = bottom)
 * @param highNote MIDI note at top of sequencing plane (0-127)
 * @param lowNote MIDI note at bottom of sequencing plane (0-127)
 * @param pitchOffset Additional pitch offset in semitones (can be fractional)
 * @return MIDI note number (0-127), clamped to valid range
 */
inline int mapVerticalPositionToNote(float normalizedY, int highNote, int lowNote, float pitchOffset)
{
    // Linear interpolation between high and low notes
    float noteFloat = highNote + (lowNote - highNote) * normalizedY + pitchOffset;
    int note = static_cast<int>(std::round(noteFloat));
    
    // Clamp to valid MIDI range
    return std::clamp(note, 0, 127);
}

/**
 * Map square height to MIDI velocity
 * @param normalizedHeight Normalized vertical height (0.0 to 1.0)
 * @return MIDI velocity (1-127), clamped to valid range
 */
inline int mapHeightToVelocity(float normalizedHeight)
{
    // Map height to velocity range 1-127
    int velocity = static_cast<int>(std::round(1.0f + normalizedHeight * 126.0f));
    return std::clamp(velocity, 1, 127);
}

} // namespace SquareBeats

#pragma once

#include "DataStructures.h"
#include "ConversionUtils.h"
#include <juce_audio_basics/juce_audio_basics.h>

namespace SquareBeats {

//==============================================================================
/**
 * MIDI Generator
 * 
 * Static utility class for generating MIDI messages from square properties.
 * Handles note calculation, velocity mapping, quantization, and MIDI message creation.
 */
class MIDIGenerator {
public:
    /**
     * Calculate MIDI note number from square position and color channel config
     * @param square The square to calculate note for
     * @param config Color channel configuration with pitch range
     * @param pitchOffset Additional pitch offset from pitch sequencer (in semitones)
     * @param scaleConfig Scale configuration for snapping notes to scale
     * @return MIDI note number (0-127), clamped to valid range
     */
    static int calculateMidiNote(const Square& square, 
                                 const ColorChannelConfig& config,
                                 float pitchOffset,
                                 const ScaleConfig& scaleConfig = ScaleConfig());
    
    /**
     * Calculate MIDI velocity from square height
     * @param square The square to calculate velocity for
     * @return MIDI velocity (1-127), clamped to valid range
     */
    static int calculateVelocity(const Square& square);
    
    /**
     * Apply quantization to a time value in beats
     * @param timeBeats Time position in beats
     * @param quantize Quantization setting
     * @param timeSig Time signature for calculating quantization intervals
     * @return Quantized time in beats
     */
    static double applyQuantization(double timeBeats, 
                                   QuantizationValue quantize,
                                   const TimeSignature& timeSig);
    
    /**
     * Create a MIDI note-on message
     * @param channel MIDI channel (1-16)
     * @param note MIDI note number (0-127)
     * @param velocity MIDI velocity (1-127)
     * @return MIDI note-on message
     */
    static juce::MidiMessage createNoteOn(int channel, int note, int velocity);
    
    /**
     * Create a MIDI note-off message
     * @param channel MIDI channel (1-16)
     * @param note MIDI note number (0-127)
     * @return MIDI note-off message
     */
    static juce::MidiMessage createNoteOff(int channel, int note);
};

} // namespace SquareBeats

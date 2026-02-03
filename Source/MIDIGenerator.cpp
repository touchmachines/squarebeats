#include "MIDIGenerator.h"

namespace SquareBeats {

//==============================================================================
int MIDIGenerator::calculateMidiNote(const Square& square, 
                                     const ColorChannelConfig& config,
                                     float pitchOffset,
                                     const ScaleConfig& scaleConfig)
{
    // Use the center Y position of the square for pitch calculation
    float normalizedY = square.getCenterY();
    
    // Use the conversion utility to map position to note with pitch offset
    int midiNote = mapVerticalPositionToNote(normalizedY, config.highNote, config.lowNote, pitchOffset);
    
    // Apply scale snapping
    return scaleConfig.snapToScale(midiNote);
}

//==============================================================================
int MIDIGenerator::calculateVelocity(const Square& square)
{
    // Use the conversion utility to map height to velocity
    return mapHeightToVelocity(square.height);
}

//==============================================================================
double MIDIGenerator::applyQuantization(double timeBeats, 
                                       QuantizationValue quantize,
                                       const TimeSignature& timeSig)
{
    double beatsPerBar = timeSig.getBeatsPerBar();
    double quantizeInterval;
    
    // Calculate quantization interval based on quantization value
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
            quantizeInterval = beatsPerBar / 16.0; // Default to 1/16
            break;
    }
    
    // Round to nearest quantization interval
    return std::round(timeBeats / quantizeInterval) * quantizeInterval;
}

//==============================================================================
juce::MidiMessage MIDIGenerator::createNoteOn(int channel, int note, int velocity)
{
    // JUCE MIDI channels are 1-16, note and velocity are 0-127
    return juce::MidiMessage::noteOn(channel, note, static_cast<juce::uint8>(velocity));
}

//==============================================================================
juce::MidiMessage MIDIGenerator::createNoteOff(int channel, int note)
{
    // JUCE MIDI channels are 1-16, note is 0-127
    return juce::MidiMessage::noteOff(channel, note);
}

} // namespace SquareBeats

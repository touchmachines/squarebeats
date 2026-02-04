#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "DataStructures.h"
#include "PatternModel.h"
#include "MIDIGenerator.h"
#include <map>

namespace SquareBeats {

//==============================================================================
/**
 * PlaybackEngine handles tempo-synchronized playback and MIDI generation
 * 
 * Responsibilities:
 * - Synchronize with host DAW transport (tempo, play/stop state)
 * - Advance playback position based on tempo
 * - Handle loop boundaries
 * - Detect square triggers and generate MIDI events
 * - Manage monophonic voice allocation per color channel
 */
class PlaybackEngine {
public:
    //==============================================================================
    PlaybackEngine();
    ~PlaybackEngine() = default;
    
    //==============================================================================
    /**
     * Set the pattern model to use for playback
     */
    void setPatternModel(PatternModel* model);
    
    /**
     * Handle transport state changes from host DAW
     * @param isPlaying Whether transport is playing
     * @param sampleRate Current sample rate
     * @param bpm Tempo in beats per minute
     * @param timeInSamples Current time in samples
     * @param timeInBeats Current time in beats
     */
    void handleTransportChange(bool isPlaying, double sampleRate, double bpm, 
                               double timeInSamples, double timeInBeats);
    
    /**
     * Process an audio block and generate MIDI messages
     * @param buffer Audio buffer (not modified, MIDI only)
     * @param midiMessages MIDI buffer to add generated messages to
     */
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    
    /**
     * Get the current playback position as a normalized value (0.0 to 1.0)
     * Thread-safe for reading from UI thread
     */
    float getNormalizedPlaybackPosition() const;
    
    /**
     * Get the current pitch sequencer position for a specific color as a normalized value (0.0 to 1.0)
     * @param colorId Color channel ID (0-3)
     */
    float getNormalizedPitchSeqPosition(int colorId) const;
    
    /**
     * Get the current scale sequencer position as a normalized value (0.0 to 1.0)
     */
    float getNormalizedScaleSeqPosition() const;
    
    /**
     * Get the current playback position in bars (for scale sequencer)
     */
    double getPositionInBars() const;
    
    /**
     * Check if playback is currently active
     */
    bool getIsPlaying() const { return isPlaying; }
    
private:
    //==============================================================================
    /**
     * Active note information for monophonic voice management
     */
    struct ActiveNote {
        int midiNote;        // MIDI note number currently playing
        int colorChannelId;  // Color channel this note belongs to
        double endTime;      // When to send note-off (in beats, normalized to loop)
    };
    
    //==============================================================================
    // Data members
    PatternModel* pattern;
    double currentPositionBeats;  // Current playback position in beats (wrapped to main loop)
    double absolutePositionBeats; // Absolute playback position in beats (for pitch sequencer)
    double loopLengthBeats;       // Loop length in beats
    bool isPlaying;               // Transport play state
    double sampleRate;            // Current sample rate
    double bpm;                   // Current tempo
    
    // Play mode state
    int currentStepIndex;         // Current step index for step-based modes
    int totalSteps;               // Total steps in the loop (based on quantization)
    bool pendulumForward;         // Current direction in pendulum mode
    juce::Random randomGenerator; // For probability mode
    
    // Monophonic voice management: one active note per color channel
    std::map<int, ActiveNote> activeNotesByColor;
    
    //==============================================================================
    // Helper methods
    
    /**
     * Update playback position based on buffer size and tempo
     * @param numSamples Number of samples in current buffer
     */
    void updatePlaybackPosition(int numSamples);
    
    /**
     * Process square triggers in the current time range
     * @param midiMessages MIDI buffer to add messages to
     * @param startBeats Start of time range (in beats)
     * @param endBeats End of time range (in beats)
     */
    void processSquareTriggers(juce::MidiBuffer& midiMessages, double startBeats, double endBeats);
    
    /**
     * Send note-off for a color channel
     * @param midiMessages MIDI buffer to add message to
     * @param colorId Color channel ID
     * @param sampleOffset Sample offset within buffer
     */
    void sendNoteOff(juce::MidiBuffer& midiMessages, int colorId, int sampleOffset);
    
    /**
     * Send note-on for a square
     * @param midiMessages MIDI buffer to add message to
     * @param square Square to trigger
     * @param sampleOffset Sample offset within buffer
     */
    void sendNoteOn(juce::MidiBuffer& midiMessages, const Square& square, int sampleOffset);
    
    /**
     * Calculate sample offset for a time in beats
     * @param timeBeats Time in beats
     * @param blockStartBeats Start time of current block in beats
     * @param numSamples Number of samples in current block
     * @return Sample offset within block
     */
    int calculateSampleOffset(double timeBeats, double blockStartBeats, int numSamples) const;
    
    /**
     * Send note-offs for all active notes
     * @param midiMessages MIDI buffer to add messages to
     */
    void stopAllNotes(juce::MidiBuffer& midiMessages);
    
    /**
     * Calculate the next step index based on play mode
     * @return The next step index
     */
    int calculateNextStep();
    
    /**
     * Get the number of steps per loop based on finest quantization
     */
    int calculateTotalSteps() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaybackEngine)
};

} // namespace SquareBeats

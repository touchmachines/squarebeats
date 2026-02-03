#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "DataStructures.h"
#include "PatternModel.h"
#include "PlaybackEngine.h"
#include "StateManager.h"

//==============================================================================
/**
 * SquareBeats VST3 Audio Processor
 * 
 * This is the main plugin processor class that handles:
 * - Audio processing callbacks
 * - MIDI generation
 * - State serialization/deserialization
 * - Communication with the editor UI
 */
class SquareBeatsAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    SquareBeatsAudioProcessor();
    ~SquareBeatsAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Access to pattern model for UI
    SquareBeats::PatternModel& getPatternModel() { return patternModel; }
    
    // Access to playback engine for UI (playback position indicator)
    SquareBeats::PlaybackEngine& getPlaybackEngine() { return playbackEngine; }

private:
    //==============================================================================
    // Core components
    SquareBeats::PatternModel patternModel;
    SquareBeats::PlaybackEngine playbackEngine;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquareBeatsAudioProcessor)
};

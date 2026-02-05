#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "DataStructures.h"
#include "PatternModel.h"
#include "PlaybackEngine.h"
#include "StateManager.h"
#include "VisualFeedback.h"
#include "PresetManager.h"

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
    
    // Access to visual feedback state for UI
    SquareBeats::VisualFeedbackState& getVisualFeedbackState() { return visualFeedbackState; }
    
    // Access to beat pulse state for UI
    SquareBeats::BeatPulseState& getBeatPulseState() { return beatPulseState; }
    
    //==============================================================================
    // Preset management
    
    /**
     * Get list of available presets
     */
    juce::StringArray getPresetList() { return presetManager.getPresetList(); }
    
    /**
     * Save current state as a named preset
     */
    bool savePreset(const juce::String& presetName) { return presetManager.savePreset(patternModel, presetName); }
    
    /**
     * Load a preset by name
     */
    bool loadPreset(const juce::String& presetName) { return presetManager.loadPreset(patternModel, presetName); }
    
    /**
     * Delete a preset by name
     */
    bool deletePreset(const juce::String& presetName) { return presetManager.deletePreset(presetName); }
    
    /**
     * Check if a preset exists
     */
    bool presetExists(const juce::String& presetName) { return presetManager.presetExists(presetName); }

private:
    //==============================================================================
    // Core components
    SquareBeats::PatternModel patternModel;
    SquareBeats::PlaybackEngine playbackEngine;
    SquareBeats::VisualFeedbackState visualFeedbackState;
    SquareBeats::BeatPulseState beatPulseState;
    SquareBeats::PresetManager presetManager;
    
    // Beat tracking for visual pulse
    double lastBeatPosition = -1.0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquareBeatsAudioProcessor)
};

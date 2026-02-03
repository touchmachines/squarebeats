#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "SequencingPlaneComponent.h"
#include "ColorSelectorComponent.h"
#include "ColorConfigPanel.h"
#include "LoopLengthSelector.h"
#include "TimeSignatureControls.h"
#include "ScaleControls.h"
#include "ControlButtons.h"
#include "PitchSequencerComponent.h"

//==============================================================================
/**
 * SquareBeats Plugin Editor
 * 
 * This is the main UI component that will contain:
 * - Sequencing plane for drawing squares
 * - Pitch sequencer overlay
 * - Color channel selector
 * - Configuration panels
 * - Loop length and time signature controls
 */
class SquareBeatsAudioProcessorEditor : public juce::AudioProcessorEditor,
                                         public SquareBeats::ColorSelectorComponent::Listener,
                                         public SquareBeats::ControlButtons::Listener,
                                         private juce::ChangeListener,
                                         private juce::Timer
{
public:
    SquareBeatsAudioProcessorEditor (SquareBeatsAudioProcessor&);
    ~SquareBeatsAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // ColorSelectorComponent::Listener
    void colorChannelSelected(int colorChannelId) override;
    
    // ControlButtons::Listener
    void pitchSequencerVisibilityChanged(bool isVisible) override;
    
    // ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    // Timer for playback position updates
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SquareBeatsAudioProcessor& audioProcessor;
    
    // UI Components
    std::unique_ptr<SquareBeats::SequencingPlaneComponent> sequencingPlane;
    std::unique_ptr<SquareBeats::PitchSequencerComponent> pitchSequencer;
    std::unique_ptr<SquareBeats::ColorSelectorComponent> colorSelector;
    std::unique_ptr<SquareBeats::ColorConfigPanel> colorConfigPanel;
    std::unique_ptr<SquareBeats::LoopLengthSelector> loopLengthSelector;
    std::unique_ptr<SquareBeats::TimeSignatureControls> timeSignatureControls;
    std::unique_ptr<SquareBeats::ScaleControls> scaleControls;
    std::unique_ptr<SquareBeats::ControlButtons> controlButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquareBeatsAudioProcessorEditor)
};

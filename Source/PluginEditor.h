#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "SequencingPlaneComponent.h"
#include "ColorSelectorComponent.h"
#include "ColorConfigPanel.h"
#include "LoopLengthSelector.h"
#include "ScaleControls.h"
#include "ControlButtons.h"
#include "PitchSequencerComponent.h"
#include "PlayModeControls.h"
#include "ScaleSequencerComponent.h"
#include "GateFlashOverlay.h"

//==============================================================================
/**
 * SquareBeats Plugin Editor
 * 
 * This is the main UI component that will contain:
 * - Logo in top left
 * - Sequencing plane for drawing squares
 * - Pitch sequencer overlay
 * - Color channel selector
 * - Configuration panels (context-sensitive based on editing mode)
 * - Loop length and time signature controls
 * - Play mode buttons in top bar
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
    void onClearAllClicked();
    void updateContextSensitiveControls();
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SquareBeatsAudioProcessor& audioProcessor;
    
    // Logo image
    juce::Image logoImage;
    
    // UI Components
    std::unique_ptr<SquareBeats::GateFlashOverlay> gateFlashOverlay;
    std::unique_ptr<SquareBeats::SequencingPlaneComponent> sequencingPlane;
    std::unique_ptr<SquareBeats::PitchSequencerComponent> pitchSequencer;
    std::unique_ptr<SquareBeats::ColorSelectorComponent> colorSelector;
    std::unique_ptr<SquareBeats::ColorConfigPanel> colorConfigPanel;
    std::unique_ptr<SquareBeats::LoopLengthSelector> loopLengthSelector;
    std::unique_ptr<SquareBeats::ScaleControls> scaleControls;
    std::unique_ptr<SquareBeats::ControlButtons> controlButtons;
    std::unique_ptr<SquareBeats::PlayModeButtons> playModeButtons;  // New: buttons in top bar
    std::unique_ptr<SquareBeats::PlayModeXYPad> playModeXYPad;      // New: XY pad in side panel
    std::unique_ptr<SquareBeats::ScaleSequencerComponent> scaleSequencer;
    
    // Scale sequencer toggle button
    juce::TextButton scaleSeqToggle;
    
    // Top bar clear all button
    juce::TextButton clearAllButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquareBeatsAudioProcessorEditor)
};

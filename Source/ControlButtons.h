#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * ControlButtons - Control buttons for pattern operations
 * 
 * Provides:
 * - "Clear All" button to clear all squares in the current color channel
 * - Pitch sequencer show/hide toggle button
 */
class ControlButtons : public juce::Component
{
public:
    //==============================================================================
    ControlButtons(PatternModel& model);
    ~ControlButtons() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    /**
     * Set the currently selected color channel (for Clear All button)
     */
    void setSelectedColorChannel(int colorChannelId);
    
    /**
     * Get the currently selected color channel
     */
    int getSelectedColorChannel() const { return selectedColorChannel; }
    
    /**
     * Refresh the UI to show current pitch sequencer visibility state
     */
    void refreshFromModel();
    
    /**
     * Listener interface for pitch sequencer visibility changes
     */
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void pitchSequencerVisibilityChanged(bool isVisible) = 0;
    };
    
    /**
     * Add a listener for pitch sequencer visibility changes
     */
    void addListener(Listener* listener);
    
    /**
     * Remove a listener
     */
    void removeListener(Listener* listener);

private:
    PatternModel& patternModel;
    int selectedColorChannel;
    std::vector<Listener*> listeners;
    
    // UI Components
    juce::TextButton clearAllButton;
    juce::TextButton pitchSequencerToggle;
    juce::TextButton clearPitchSequencerButton;
    juce::ComboBox pitchSeqLengthCombo;
    juce::Label pitchSeqLengthLabel;
    
    /**
     * Initialize UI components
     */
    void setupComponents();
    
    /**
     * Handle Clear All button click
     */
    void onClearAllClicked();
    
    /**
     * Handle pitch sequencer toggle click
     */
    void onPitchSequencerToggleClicked();
    
    /**
     * Handle clear pitch sequencer button click
     */
    void onClearPitchSequencerClicked();
    
    /**
     * Handle pitch sequencer length change
     */
    void onPitchSeqLengthChanged();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlButtons)
};

} // namespace SquareBeats

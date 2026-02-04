#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * ColorConfigPanel - Configuration panel for a color channel
 * 
 * Tab-based panel with two modes:
 * - SQUARES tab: Quantization, High/Low note, MIDI channel
 * - PITCH tab: Pitch sequencer editing mode
 */
class ColorConfigPanel : public juce::Component
{
public:
    //==============================================================================
    ColorConfigPanel(PatternModel& model);
    ~ColorConfigPanel() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    /**
     * Set which color channel this panel is configuring
     */
    void setColorChannel(int colorChannelId);
    
    /**
     * Get the current color channel being configured
     */
    int getColorChannel() const { return currentColorChannel; }
    
    /**
     * Refresh the UI to show current configuration values
     */
    void refreshFromModel();
    
    //==============================================================================
    /**
     * Callback for when editing mode changes (SQUARES vs PITCH tab)
     * Set this to be notified when the user switches tabs
     */
    std::function<void(bool isPitchMode)> onEditingModeChanged;

private:
    PatternModel& patternModel;
    int currentColorChannel;
    
    // Tab buttons
    juce::TextButton notesTabButton;
    juce::TextButton pitchTabButton;
    
    // Note mode UI Components
    juce::Label quantizationLabel;
    juce::ComboBox quantizationCombo;
    
    juce::Label highNoteLabel;
    juce::Slider highNoteSlider;
    juce::Label highNoteValue;
    
    juce::Label lowNoteLabel;
    juce::Slider lowNoteSlider;
    juce::Label lowNoteValue;
    
    juce::Label midiChannelLabel;
    juce::ComboBox midiChannelCombo;
    
    // Pitch sequencer controls (always visible)
    juce::Label pitchSeqLengthLabel;
    juce::ComboBox pitchSeqLengthCombo;
    
    // Context-sensitive clear button
    juce::TextButton clearButton;
    
    /**
     * Initialize UI components
     */
    void setupComponents();
    
    /**
     * Update visibility of controls based on active tab
     */
    void updateControlVisibility();
    
    /**
     * Switch to Notes tab
     */
    void switchToNotesTab();
    
    /**
     * Switch to Pitch tab
     */
    void switchToPitchTab();
    
    /**
     * Handle clear button click (context-sensitive)
     */
    void onClearClicked();
    
    /**
     * Handle quantization combo box change
     */
    void onQuantizationChanged();
    
    /**
     * Handle high note slider change
     */
    void onHighNoteChanged();
    
    /**
     * Handle low note slider change
     */
    void onLowNoteChanged();
    
    /**
     * Handle MIDI channel combo box change
     */
    void onMidiChannelChanged();
    
    /**
     * Handle pitch sequencer length change
     */
    void onPitchSeqLengthChanged();
    
    /**
     * Convert MIDI note number to note name (e.g., 60 -> "C4")
     */
    static juce::String midiNoteToName(int midiNote);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorConfigPanel)
};

} // namespace SquareBeats

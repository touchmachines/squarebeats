#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * ColorConfigPanel - Configuration panel for a color channel
 * 
 * Context-sensitive panel that shows:
 * - In Square mode: Quantization, High/Low note, MIDI channel
 * - In Pitch Sequencer mode: Pitch range (semitones), Pitch Len
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

private:
    PatternModel& patternModel;
    int currentColorChannel;
    
    // Section header label
    juce::Label sectionHeader;
    
    // Square mode UI Components
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
    
    /**
     * Initialize UI components
     */
    void setupComponents();
    
    /**
     * Update visibility of controls based on editing mode
     */
    void updateControlVisibility();
    
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
     * Convert MIDI note number to note name (e.g., 60 -> "C4")
     */
    static juce::String midiNoteToName(int midiNote);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorConfigPanel)
};

} // namespace SquareBeats

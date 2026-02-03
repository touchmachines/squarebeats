#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * TimeSignatureControls - UI for configuring time signature
 * 
 * Displays input fields for numerator (1-16) and denominator (1, 2, 4, 8, 16).
 * Validates input ranges and updates the PatternModel when values change.
 */
class TimeSignatureControls : public juce::Component
{
public:
    //==============================================================================
    TimeSignatureControls(PatternModel& model);
    ~TimeSignatureControls() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    /**
     * Refresh the UI to show current time signature from model
     */
    void refreshFromModel();

private:
    PatternModel& patternModel;
    
    // UI Components
    juce::Label titleLabel;
    juce::Label numeratorLabel;
    juce::ComboBox numeratorCombo;
    juce::Label separatorLabel;
    juce::Label denominatorLabel;
    juce::ComboBox denominatorCombo;
    
    /**
     * Initialize UI components
     */
    void setupComponents();
    
    /**
     * Handle numerator change
     */
    void onNumeratorChanged();
    
    /**
     * Handle denominator change
     */
    void onDenominatorChanged();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeSignatureControls)
};

} // namespace SquareBeats

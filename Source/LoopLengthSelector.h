#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * LoopLengthSelector - UI for selecting loop length
 * 
 * Displays a dropdown for selecting loop length from 1-64 bars.
 * Updates the PatternModel when the selection changes.
 */
class LoopLengthSelector : public juce::Component
{
public:
    //==============================================================================
    LoopLengthSelector(PatternModel& model);
    ~LoopLengthSelector() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    /**
     * Refresh the UI to show current loop length from model
     */
    void refreshFromModel();

private:
    PatternModel& patternModel;
    
    // UI Components
    juce::Label loopLengthLabel;
    juce::ComboBox loopLengthCombo;
    
    /**
     * Handle loop length combo box change
     */
    void onLoopLengthChanged();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoopLengthSelector)
};

} // namespace SquareBeats

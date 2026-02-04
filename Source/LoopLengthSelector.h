#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * LoopLengthSelector - UI for selecting loop length
 * 
 * Displays a dropdown for selecting loop length:
 * - 1-15 steps (1/16 bar each)
 * - 1-8 bars
 * - 16, 32, 64 bars
 * 
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
    
    /**
     * Convert combo box item ID to loop length in bars
     * Item IDs 1-15: steps (1/16 bar each)
     * Item IDs 16-23: 1-8 bars
     * Item IDs 24-26: 16, 32, 64 bars
     */
    double itemIdToLoopLength(int itemId) const;
    
    /**
     * Convert loop length in bars to closest combo box item ID
     */
    int loopLengthToItemId(double loopLengthBars) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoopLengthSelector)
};

} // namespace SquareBeats

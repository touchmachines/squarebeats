#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * LoopLengthSelector - UI for selecting loop length
 * 
 * Displays buttons for 1, 2, and 4 bar loop lengths.
 * Highlights the currently selected length and updates the PatternModel
 * when the selection changes.
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
    void mouseDown(const juce::MouseEvent& event) override;
    
    //==============================================================================
    /**
     * Refresh the UI to show current loop length from model
     */
    void refreshFromModel();

private:
    PatternModel& patternModel;
    
    // Available loop lengths
    static constexpr int NUM_LOOP_OPTIONS = 3;
    static constexpr int LOOP_OPTIONS[NUM_LOOP_OPTIONS] = { 1, 2, 4 };
    
    /**
     * Get the bounds for a specific loop length button
     */
    juce::Rectangle<int> getButtonBounds(int buttonIndex) const;
    
    /**
     * Find which button was clicked at the given position
     * @return Button index (0-2) or -1 if no button was clicked
     */
    int findButtonAt(juce::Point<int> position) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoopLengthSelector)
};

} // namespace SquareBeats

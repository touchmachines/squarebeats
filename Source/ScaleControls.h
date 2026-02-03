#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * ScaleControls - UI component for selecting root note and scale type
 * 
 * Provides two dropdown menus:
 * - Root note selection (C through B)
 * - Scale type selection (Chromatic, Major, Minor, etc.)
 */
class ScaleControls : public juce::Component,
                      private juce::ComboBox::Listener
{
public:
    ScaleControls(PatternModel& model);
    ~ScaleControls() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /**
     * Refresh UI from model (call after external model changes)
     */
    void refreshFromModel();
    
private:
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    
    PatternModel& patternModel;
    
    juce::Label rootLabel;
    juce::ComboBox rootNoteCombo;
    
    juce::Label scaleLabel;
    juce::ComboBox scaleTypeCombo;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleControls)
};

} // namespace SquareBeats

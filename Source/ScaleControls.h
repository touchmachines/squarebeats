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
 * 
 * Can also display an "active" scale when scale sequencer is enabled
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
    
    /**
     * Set the currently active scale (for display when scale sequencer is running)
     * Pass nullptr to clear and show the static scale
     */
    void setActiveScale(const ScaleConfig* activeScale);
    
    /**
     * Enable or disable the manual scale controls
     * When disabled, shows the active scale from scale sequencer instead
     */
    void setControlsEnabled(bool enabled);
    
private:
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    
    PatternModel& patternModel;
    
    juce::Label rootLabel;
    juce::ComboBox rootNoteCombo;
    
    juce::Label scaleLabel;
    juce::ComboBox scaleTypeCombo;
    
    // Active scale display (when scale sequencer is enabled)
    juce::Label activeScaleLabel;
    bool showingActiveScale = false;
    bool controlsEnabled = true;
    ScaleConfig currentActiveScale;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleControls)
};

} // namespace SquareBeats

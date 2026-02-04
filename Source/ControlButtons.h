#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * ControlButtons - Control buttons for pattern operations
 * 
 * Note: Most controls have been moved to ColorConfigPanel with tab-based UI.
 * This component is kept for backward compatibility but may be removed.
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
     * Set the currently selected color channel
     */
    void setSelectedColorChannel(int colorChannelId);
    
    /**
     * Get the currently selected color channel
     */
    int getSelectedColorChannel() const { return selectedColorChannel; }
    
    /**
     * Refresh the UI to show current state
     */
    void refreshFromModel();
    
    /**
     * Listener interface for pitch sequencer visibility changes
     * @deprecated Use ColorConfigPanel::onEditingModeChanged instead
     */
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void pitchSequencerVisibilityChanged(bool isVisible) = 0;
    };
    
    /**
     * Add a listener for pitch sequencer visibility changes
     * @deprecated Use ColorConfigPanel::onEditingModeChanged instead
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
    
    // This component is now mostly empty - controls moved to ColorConfigPanel
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlButtons)
};

} // namespace SquareBeats

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"
#include "VisualFeedback.h"

namespace SquareBeats {

//==============================================================================
/**
 * ColorSelectorComponent - UI for selecting the active color channel
 * 
 * Displays 4 color buttons representing the available color channels.
 * Highlights the currently selected color and notifies listeners when
 * the selection changes.
 */
class ColorSelectorComponent : public juce::Component
{
public:
    //==============================================================================
    /**
     * Listener interface for color selection changes
     */
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void colorChannelSelected(int colorChannelId) = 0;
    };
    
    //==============================================================================
    ColorSelectorComponent(PatternModel& model);
    ~ColorSelectorComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    
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
     * Add a listener for color selection changes
     */
    void addListener(Listener* listener);
    
    /**
     * Remove a listener
     */
    void removeListener(Listener* listener);
    
    /**
     * Set the visual feedback state for activity indicators
     */
    void setVisualFeedbackState(VisualFeedbackState* state) { visualFeedback = state; }

private:
    PatternModel& patternModel;
    int selectedColorChannel;
    juce::ListenerList<Listener> listeners;
    VisualFeedbackState* visualFeedback = nullptr;
    
    /**
     * Get the bounds for a specific color button
     */
    juce::Rectangle<int> getColorButtonBounds(int colorChannelId) const;
    
    /**
     * Find which color button was clicked at the given position
     * @return Color channel ID (0-3) or -1 if no button was clicked
     */
    int findColorButtonAt(juce::Point<int> position) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorSelectorComponent)
};

} // namespace SquareBeats

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"
#include "VisualFeedback.h"

namespace SquareBeats {

//==============================================================================
/**
 * ScaleSequencerComponent - Timeline editor for scale/key sequences
 * 
 * Displays a horizontal timeline of scale segments that can be:
 * - Added with a "+" button
 * - Edited by clicking (shows popup editor)
 * - Deleted with an "X" button on each segment
 * - Resized by dragging segment edges
 */
class ScaleSequencerComponent : public juce::Component
{
public:
    ScaleSequencerComponent(PatternModel& model);
    ~ScaleSequencerComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    
    /**
     * Set the current playback position (normalized 0.0 to 1.0 within scale sequence)
     */
    void setPlaybackPosition(float normalizedPosition);
    
    /**
     * Refresh UI from model
     */
    void refreshFromModel();
    
    /**
     * Set visual feedback state for scale change flash
     */
    void setVisualFeedbackState(VisualFeedbackState* state) { visualFeedback = state; }
    
    /**
     * Listener interface for visibility changes
     */
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void scaleSequencerVisibilityChanged(bool isVisible) = 0;
    };
    
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    PatternModel& patternModel;
    float playbackPosition = 0.0f;
    int lastActiveSegment = -1;  // For detecting segment changes
    float segmentChangeFlash = 0.0f;  // Flash intensity on segment change
    VisualFeedbackState* visualFeedback = nullptr;
    
    // Interaction state
    int hoveredSegment = -1;
    int selectedSegment = -1;
    int draggingEdge = -1;  // Index of segment whose right edge is being dragged
    float dragStartX = 0.0f;
    int dragStartBars = 0;
    
    // UI Components
    juce::TextButton addButton;
    
    // Popup editor components
    std::unique_ptr<juce::Component> popupEditor;
    std::unique_ptr<juce::ComboBox> popupRootCombo;
    std::unique_ptr<juce::ComboBox> popupScaleCombo;
    std::unique_ptr<juce::ComboBox> popupBarsCombo;
    std::unique_ptr<juce::TextButton> popupDeleteButton;
    std::unique_ptr<juce::TextButton> popupCloseButton;
    
    std::vector<Listener*> listeners;
    
    // Helper methods
    juce::Rectangle<float> getSegmentBounds(int index) const;
    int getSegmentAtPoint(juce::Point<float> point) const;
    int getEdgeAtPoint(juce::Point<float> point) const;  // Returns segment index if near right edge
    float barsToPixelX(int bars) const;
    int pixelXToBars(float x) const;
    
    void showSegmentEditor(int segmentIndex);
    void hideSegmentEditor();
    void onAddSegment();
    void onDeleteSegment(int index);
    void onSegmentChanged(int index);
    
    juce::Colour getSegmentColor(int index) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleSequencerComponent)
};

} // namespace SquareBeats

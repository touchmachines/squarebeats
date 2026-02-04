#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"
#include "VisualFeedback.h"

namespace SquareBeats {

//==============================================================================
/**
 * SequencingPlaneComponent - The main visual canvas for drawing and editing squares
 * 
 * This component:
 * - Renders grid lines for visual reference
 * - Renders all squares from the PatternModel
 * - Handles mouse interaction for creating, moving, and resizing squares
 * - Displays the playback position indicator
 * 
 * All rendering uses normalized coordinates (0.0 to 1.0) which are scaled
 * to the component's actual pixel dimensions.
 */
class SequencingPlaneComponent : public juce::Component
{
public:
    //==============================================================================
    SequencingPlaneComponent(PatternModel& model);
    ~SequencingPlaneComponent() override;

    //==============================================================================
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Mouse interaction overrides
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    bool keyPressed(const juce::KeyPress& key) override;
    
    //==============================================================================
    // Playback position
    
    /**
     * Set the current playback position (normalized 0.0 to 1.0)
     */
    void setPlaybackPosition(float normalizedPosition);
    
    /**
     * Set the playback position for a specific color channel
     */
    void setColorPlaybackPosition(int colorId, float normalizedPosition);
    
    /**
     * Get the current playback position
     */
    float getPlaybackPosition() const { return playbackPosition; }
    
    //==============================================================================
    // Color channel selection
    
    /**
     * Set the currently selected color channel for new squares
     */
    void setSelectedColorChannel(int colorId);
    
    /**
     * Get the currently selected color channel
     */
    int getSelectedColorChannel() const { return selectedColorChannel; }
    
    /**
     * Set the visual feedback state for active square glow effects
     */
    void setVisualFeedbackState(VisualFeedbackState* state) { visualFeedback = state; }
    
    /**
     * Set the beat pulse intensity for grid breathing effect
     */
    void setBeatPulseIntensity(float intensity) { beatPulseIntensity = juce::jlimit(0.0f, 1.0f, intensity); }

protected:
    //==============================================================================
    // Rendering helpers (protected for testing)
    
    /**
     * Draw grid lines for visual reference
     */
    void drawGridLines(juce::Graphics& g);
    
    /**
     * Draw all squares from the pattern model
     */
    void drawSquares(juce::Graphics& g);
    
    /**
     * Draw the playback position indicator
     */
    void drawPlaybackIndicator(juce::Graphics& g);
    
    /**
     * Draw glow effect around active (playing) squares
     */
    void drawActiveSquareGlow(juce::Graphics& g);
    
    /**
     * Convert normalized coordinates to pixel coordinates
     */
    juce::Rectangle<float> normalizedToPixels(float left, float top, float width, float height) const;
    
    /**
     * Convert pixel X coordinate to normalized time
     */
    float pixelXToNormalized(float pixelX) const;
    
    /**
     * Convert pixel Y coordinate to normalized vertical position
     */
    float pixelYToNormalized(float pixelY) const;

private:
    //==============================================================================
    // Data members
    PatternModel& patternModel;
    float playbackPosition;
    float colorPlaybackPositions[4];  // Per-color playback positions
    int selectedColorChannel;
    
    // Mouse interaction state
    bool isCreatingSquare;
    juce::Point<float> dragStartPoint;
    Square* currentlyCreatingSquare;
    
    // Editing state
    enum class EditMode {
        None,
        Creating,
        Moving,
        ResizingLeft,
        ResizingRight,
        ResizingTop,
        ResizingBottom,
        ResizingTopLeft,
        ResizingTopRight,
        ResizingBottomLeft,
        ResizingBottomRight
    };
    
    EditMode currentEditMode;
    Square* selectedSquare;
    juce::Point<float> editStartMousePos;
    float editStartLeft, editStartTop, editStartWidth, editStartHeight;
    
    // Visual feedback state (optional, for active square glow)
    VisualFeedbackState* visualFeedback = nullptr;
    
    // Beat pulse intensity (0.0 to 1.0, set from timer)
    float beatPulseIntensity = 0.0f;
    
    /**
     * Hit test to find a square at the given normalized coordinates
     * @return Pointer to the square, or nullptr if no square found
     */
    Square* findSquareAt(float normalizedX, float normalizedY);
    
    /**
     * Determine the edit mode based on where the mouse is within a square
     */
    EditMode determineEditMode(const Square* square, float normalizedX, float normalizedY) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequencingPlaneComponent)
};

} // namespace SquareBeats

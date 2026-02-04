#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"
#include "VisualFeedback.h"

namespace SquareBeats {

//==============================================================================
/**
 * GateFlashOverlay - Visual overlay that flashes colors when gates trigger
 * 
 * This component renders semi-transparent color washes that fade out
 * when MIDI notes are triggered. Multiple colors blend additively
 * for a layered visual effect.
 * 
 * Features:
 * - Background color flash on gate-on events
 * - Smooth exponential decay
 * - Additive color blending for multiple simultaneous triggers
 * - Beat pulse effect on grid
 */
class GateFlashOverlay : public juce::Component
{
public:
    //==============================================================================
    GateFlashOverlay(PatternModel& model, VisualFeedbackState& feedbackState);
    ~GateFlashOverlay() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    
    /**
     * Set the base opacity for flash effects (0.0 to 1.0)
     * Default is 0.15 for subtle effect
     */
    void setFlashOpacity(float opacity) { flashOpacity = juce::jlimit(0.0f, 1.0f, opacity); }
    
    /**
     * Enable/disable the beat pulse effect
     */
    void setBeatPulseEnabled(bool enabled) { beatPulseEnabled = enabled; }

private:
    PatternModel& patternModel;
    VisualFeedbackState& visualFeedback;
    
    float flashOpacity = 0.12f;  // Base opacity for flash (subtle)
    bool beatPulseEnabled = true;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GateFlashOverlay)
};

} // namespace SquareBeats

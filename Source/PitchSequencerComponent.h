#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * PitchSequencerComponent - Overlay component for pitch modulation
 * 
 * This component:
 * - Renders the pitch sequencer waveform as an overlay
 * - Handles XY drawing to record pitch modulation curves
 * - Stores waveform data in the PatternModel's PitchSequencer
 * - Shows/hides based on the PitchSequencer's visible state
 * 
 * The pitch sequencer allows continuous pitch modulation that runs
 * independently of the main square pattern loop.
 */
class PitchSequencerComponent : public juce::Component
{
public:
    //==============================================================================
    PitchSequencerComponent(PatternModel& model);
    ~PitchSequencerComponent() override;

    //==============================================================================
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Mouse interaction overrides
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    //==============================================================================
    /**
     * Update visibility based on the PitchSequencer's visible state
     */
    void updateVisibility();
    
    /**
     * Set the currently selected color channel
     */
    void setSelectedColorChannel(int colorId);
    
    /**
     * Get the currently selected color channel
     */
    int getSelectedColorChannel() const { return selectedColorChannel; }
    
    /**
     * Set the playback position for the pitch sequencer (normalized 0.0 to 1.0)
     */
    void setPlaybackPosition(float normalizedPosition);
    
    /**
     * Set the waveform resolution (number of samples)
     */
    void setWaveformResolution(int numSamples);
    
    /**
     * Get the current waveform resolution
     */
    int getWaveformResolution() const { return waveformResolution; }

protected:
    //==============================================================================
    // Rendering helpers (protected for testing)
    
    /**
     * Draw the pitch waveform
     */
    void drawWaveform(juce::Graphics& g);
    
    /**
     * Convert pixel X coordinate to normalized time (0.0 to 1.0)
     */
    float pixelXToNormalized(float pixelX) const;
    
    /**
     * Convert pixel Y coordinate to pitch offset in semitones
     * Center of component = 0 semitones
     * Top = +12 semitones (1 octave up)
     * Bottom = -12 semitones (1 octave down)
     */
    float pixelYToPitchOffset(float pixelY) const;
    
    /**
     * Convert pitch offset to pixel Y coordinate
     */
    float pitchOffsetToPixelY(float pitchOffset) const;

private:
    //==============================================================================
    // Data members
    PatternModel& patternModel;
    int waveformResolution;  // Number of samples in the waveform
    int selectedColorChannel; // Currently selected color channel
    float playbackPosition;  // Current playback position (0.0 to 1.0)
    bool isDrawing;          // True when user is actively drawing
    
    /**
     * Record a pitch offset at the given normalized position
     */
    void recordPitchOffset(float normalizedX, float pitchOffset);
    
    /**
     * Initialize waveform with default values (all zeros)
     */
    void initializeWaveform();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchSequencerComponent)
};

} // namespace SquareBeats

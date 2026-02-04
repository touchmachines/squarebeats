#pragma once

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include "DataStructures.h"
#include <vector>
#include <array>
#include <algorithm>

namespace SquareBeats {

//==============================================================================
/**
 * PatternModel manages the complete state of a SquareBeats pattern
 * 
 * This includes:
 * - All squares with their positions, sizes, and color assignments
 * - Color channel configurations (quantization, pitch range, MIDI channel)
 * - Pitch sequencer waveform data
 * - Global settings (loop length, time signature)
 * 
 * Implements ChangeBroadcaster to notify listeners when the model changes.
 */
class PatternModel : public juce::ChangeBroadcaster {
public:
    //==============================================================================
    PatternModel();
    ~PatternModel() = default;
    
    //==============================================================================
    // Square management methods
    
    /**
     * Create a new square with the specified properties
     * @return Pointer to the created square (owned by PatternModel)
     */
    Square* createSquare(float left, float top, float width, float height, int colorId);
    
    /**
     * Delete a square by its unique ID
     * @return true if square was found and deleted, false otherwise
     */
    bool deleteSquare(uint32_t squareId);
    
    /**
     * Move a square to a new position
     * @return true if square was found and moved, false otherwise
     */
    bool moveSquare(uint32_t squareId, float newLeft, float newTop);
    
    /**
     * Resize a square
     * @return true if square was found and resized, false otherwise
     */
    bool resizeSquare(uint32_t squareId, float newWidth, float newHeight);
    
    /**
     * Remove all squares of a specific color channel
     */
    void clearColorChannel(int colorId);
    
    //==============================================================================
    // Query methods for playback
    
    /**
     * Get all squares whose time range intersects [startTime, endTime]
     * Times are in normalized coordinates (0.0 to 1.0)
     */
    std::vector<Square*> getSquaresInTimeRange(float startTime, float endTime);
    
    /**
     * Get all squares in the pattern
     */
    std::vector<Square*> getAllSquares();
    
    /**
     * Get all squares (const version)
     */
    std::vector<const Square*> getAllSquares() const;
    
    //==============================================================================
    // Color channel configuration
    
    /**
     * Get color channel configuration (mutable)
     */
    ColorChannelConfig& getColorConfig(int colorId);
    
    /**
     * Get color channel configuration (const)
     */
    const ColorChannelConfig& getColorConfig(int colorId) const;
    
    /**
     * Set color channel configuration
     */
    void setColorConfig(int colorId, const ColorChannelConfig& config);
    
    //==============================================================================
    // Pitch sequencer
    
    /**
     * Get pitch sequencer (mutable)
     */
    PitchSequencer& getPitchSequencer();
    
    /**
     * Get pitch sequencer (const)
     */
    const PitchSequencer& getPitchSequencer() const;
    
    //==============================================================================
    // Play mode configuration
    
    /**
     * Get play mode configuration (mutable)
     */
    PlayModeConfig& getPlayModeConfig();
    
    /**
     * Get play mode configuration (const)
     */
    const PlayModeConfig& getPlayModeConfig() const;
    
    //==============================================================================
    // Scale sequencer configuration
    
    /**
     * Get scale sequencer configuration (mutable)
     */
    ScaleSequencerConfig& getScaleSequencer();
    
    /**
     * Get scale sequencer configuration (const)
     */
    const ScaleSequencerConfig& getScaleSequencer() const;
    
    /**
     * Get the currently active scale (considers scale sequencer if enabled)
     * @param positionBars Current playback position in bars
     */
    ScaleConfig getActiveScale(double positionBars) const;
    
    //==============================================================================
    // Global settings
    
    /**
     * Set loop length in bars (1, 2, or 4)
     */
    void setLoopLength(int bars);
    
    /**
     * Get loop length in bars
     */
    int getLoopLength() const;
    
    /**
     * Set time signature
     */
    void setTimeSignature(int numerator, int denominator);
    
    /**
     * Get time signature
     */
    TimeSignature getTimeSignature() const;
    
    /**
     * Set scale configuration (root note and scale type)
     */
    void setScaleConfig(const ScaleConfig& config);
    
    /**
     * Get scale configuration
     */
    const ScaleConfig& getScaleConfig() const;
    
private:
    //==============================================================================
    // Data members
    std::vector<Square> squares;
    std::array<ColorChannelConfig, 4> colorConfigs;
    PitchSequencer pitchSequencer;
    PlayModeConfig playModeConfig;
    ScaleSequencerConfig scaleSequencer;
    ScaleConfig scaleConfig;
    int loopLengthBars;
    TimeSignature timeSignature;
    uint32_t nextUniqueId;
    
    //==============================================================================
    // Helper methods
    
    /**
     * Find a square by its unique ID
     * @return Iterator to the square, or squares.end() if not found
     */
    std::vector<Square>::iterator findSquareById(uint32_t squareId);
    
    /**
     * Initialize default color channel configurations
     */
    void initializeDefaultColorConfigs();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternModel)
};

} // namespace SquareBeats

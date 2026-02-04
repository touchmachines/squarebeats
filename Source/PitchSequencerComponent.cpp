#include "PitchSequencerComponent.h"

namespace SquareBeats {

//==============================================================================
PitchSequencerComponent::PitchSequencerComponent(PatternModel& model)
    : patternModel(model)
    , waveformResolution(256)  // Default resolution: 256 samples
    , selectedColorChannel(0)
    , playbackPosition(0.0f)
    , isDrawing(false)
    , lastNormalizedX(0.0f)
    , lastPitchOffset(0.0f)
{
    // Waveforms are now initialized per-color in PatternModel
    
    // Update visibility based on model state (this also sets mouse interception)
    updateVisibility();
}

PitchSequencerComponent::~PitchSequencerComponent()
{
}

//==============================================================================
void PitchSequencerComponent::paint(juce::Graphics& g)
{
    auto& pitchSequencer = patternModel.getPitchSequencer();
    auto bounds = getLocalBounds().toFloat();
    
    // Always draw the waveform (pitch modulation is always active)
    // But only show the full editing overlay when in pitch editing mode
    
    if (pitchSequencer.editingPitch)
    {
        // Full editing mode: show semi-transparent background and reference lines
        g.fillAll(juce::Colour(0x40000000));
        
        // Draw center line (0 semitones)
        float centerY = bounds.getCentreY();
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawLine(bounds.getX(), centerY, bounds.getRight(), centerY, 1.0f);
        
        // Draw +/- 12 semitone reference lines
        float octaveUpY = pitchOffsetToPixelY(12.0f);
        float octaveDownY = pitchOffsetToPixelY(-12.0f);
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawLine(bounds.getX(), octaveUpY, bounds.getRight(), octaveUpY, 1.0f);
        g.drawLine(bounds.getX(), octaveDownY, bounds.getRight(), octaveDownY, 1.0f);
    }
    
    // Always draw the waveform (visible in both modes)
    drawWaveform(g);
    
    // Draw playback position as a circle moving along the waveform (no vertical bar)
    auto& colorConfig = patternModel.getColorConfig(selectedColorChannel);
    float pixelX = bounds.getX() + playbackPosition * bounds.getWidth();
    
    // Draw a larger glowing dot at the current waveform value (touch-friendly)
    if (!colorConfig.pitchWaveform.empty())
    {
        float currentPitch = colorConfig.getPitchOffsetAt(playbackPosition);
        float dotY = pitchOffsetToPixelY(currentPitch);
        
        // Large outer glow
        g.setColour(colorConfig.displayColor.withAlpha(0.3f));
        g.fillEllipse(pixelX - 20.0f, dotY - 20.0f, 40.0f, 40.0f);
        
        // Medium glow ring
        g.setColour(colorConfig.displayColor.withAlpha(0.5f));
        g.fillEllipse(pixelX - 12.0f, dotY - 12.0f, 24.0f, 24.0f);
        
        // Inner bright dot
        g.setColour(juce::Colours::white);
        g.fillEllipse(pixelX - 6.0f, dotY - 6.0f, 12.0f, 12.0f);
    }
    else
    {
        // No waveform yet - show dot at center line
        float dotY = bounds.getCentreY();
        
        g.setColour(colorConfig.displayColor.withAlpha(0.3f));
        g.fillEllipse(pixelX - 20.0f, dotY - 20.0f, 40.0f, 40.0f);
        
        g.setColour(colorConfig.displayColor.withAlpha(0.5f));
        g.fillEllipse(pixelX - 12.0f, dotY - 12.0f, 24.0f, 24.0f);
        
        g.setColour(juce::Colours::white);
        g.fillEllipse(pixelX - 6.0f, dotY - 6.0f, 12.0f, 12.0f);
    }
}

void PitchSequencerComponent::resized()
{
    // Nothing to do here - all rendering is done in paint()
}

//==============================================================================
void PitchSequencerComponent::updateVisibility()
{
    auto& pitchSequencer = patternModel.getPitchSequencer();
    
    // Component is always visible (waveform always shows)
    setVisible(true);
    
    // Only intercept mouse clicks when in pitch editing mode
    setInterceptsMouseClicks(pitchSequencer.editingPitch, false);
}

void PitchSequencerComponent::setWaveformResolution(int numSamples)
{
    if (numSamples > 0 && numSamples != waveformResolution)
    {
        waveformResolution = numSamples;
        // Waveforms are managed per-color in PatternModel
    }
}

void PitchSequencerComponent::setSelectedColorChannel(int colorId)
{
    selectedColorChannel = juce::jlimit(0, 3, colorId);
    repaint();
}

void PitchSequencerComponent::setPlaybackPosition(float normalizedPosition)
{
    if (playbackPosition != normalizedPosition)
    {
        playbackPosition = normalizedPosition;
        repaint();
    }
}

//==============================================================================
void PitchSequencerComponent::drawWaveform(juce::Graphics& g)
{
    auto& colorConfig = patternModel.getColorConfig(selectedColorChannel);
    
    if (colorConfig.pitchWaveform.empty())
    {
        return;
    }
    
    auto bounds = getLocalBounds().toFloat();
    
    // Create a path for the waveform
    juce::Path waveformPath;
    
    bool firstPoint = true;
    for (size_t i = 0; i < colorConfig.pitchWaveform.size(); ++i)
    {
        float normalizedX = static_cast<float>(i) / (colorConfig.pitchWaveform.size() - 1);
        float pixelX = bounds.getX() + normalizedX * bounds.getWidth();
        float pixelY = pitchOffsetToPixelY(colorConfig.pitchWaveform[i]);
        
        if (firstPoint)
        {
            waveformPath.startNewSubPath(pixelX, pixelY);
            firstPoint = false;
        }
        else
        {
            waveformPath.lineTo(pixelX, pixelY);
        }
    }
    
    // Draw the waveform path using the color's display color
    g.setColour(colorConfig.displayColor.withAlpha(0.8f));
    g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
    
    // Draw dots at each sample point for better visibility
    g.setColour(colorConfig.displayColor);
    for (size_t i = 0; i < colorConfig.pitchWaveform.size(); ++i)
    {
        float normalizedX = static_cast<float>(i) / (colorConfig.pitchWaveform.size() - 1);
        float pixelX = bounds.getX() + normalizedX * bounds.getWidth();
        float pixelY = pitchOffsetToPixelY(colorConfig.pitchWaveform[i]);
        
        g.fillEllipse(pixelX - 2.0f, pixelY - 2.0f, 4.0f, 4.0f);
    }
}

//==============================================================================
float PitchSequencerComponent::pixelXToNormalized(float pixelX) const
{
    auto bounds = getLocalBounds().toFloat();
    return (pixelX - bounds.getX()) / bounds.getWidth();
}

float PitchSequencerComponent::pixelYToPitchOffset(float pixelY) const
{
    auto bounds = getLocalBounds().toFloat();
    float centerY = bounds.getCentreY();
    
    // Map pixel Y to pitch offset
    // Top of component = +12 semitones
    // Center = 0 semitones
    // Bottom = -12 semitones
    float normalizedY = (pixelY - centerY) / (bounds.getHeight() * 0.5f);
    return -normalizedY * 12.0f;  // Negative because Y increases downward
}

float PitchSequencerComponent::pitchOffsetToPixelY(float pitchOffset) const
{
    auto bounds = getLocalBounds().toFloat();
    float centerY = bounds.getCentreY();
    
    // Map pitch offset to pixel Y
    float normalizedY = -pitchOffset / 12.0f;  // Negative because Y increases downward
    return centerY + normalizedY * (bounds.getHeight() * 0.5f);
}

//==============================================================================
void PitchSequencerComponent::mouseDown(const juce::MouseEvent& event)
{
    auto& pitchSequencer = patternModel.getPitchSequencer();
    
    // Only handle mouse events if in pitch editing mode
    if (!pitchSequencer.editingPitch)
    {
        return;
    }
    
    // Start drawing
    isDrawing = true;
    
    // Record the initial point
    auto mousePos = event.position;
    float normalizedX = juce::jlimit(0.0f, 1.0f, pixelXToNormalized(mousePos.x));
    float pitchOffset = juce::jlimit(-12.0f, 12.0f, pixelYToPitchOffset(mousePos.y));
    
    // Store for interpolation on subsequent drag events
    lastNormalizedX = normalizedX;
    lastPitchOffset = pitchOffset;
    
    recordPitchOffset(normalizedX, pitchOffset);
    
    repaint();
}

void PitchSequencerComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (!isDrawing)
    {
        return;
    }
    
    // Get current mouse position
    auto mousePos = event.position;
    float currentNormalizedX = juce::jlimit(0.0f, 1.0f, pixelXToNormalized(mousePos.x));
    float currentPitchOffset = juce::jlimit(-12.0f, 12.0f, pixelYToPitchOffset(mousePos.y));
    
    // Interpolate between last position and current position to fill gaps
    // This prevents choppy drawing when the mouse moves fast
    auto& colorConfig = patternModel.getColorConfig(selectedColorChannel);
    
    // Ensure waveform is initialized
    if (colorConfig.pitchWaveform.empty())
    {
        colorConfig.pitchWaveform.resize(waveformResolution, 0.0f);
    }
    
    int waveformSize = static_cast<int>(colorConfig.pitchWaveform.size());
    int lastIndex = static_cast<int>(lastNormalizedX * (waveformSize - 1));
    int currentIndex = static_cast<int>(currentNormalizedX * (waveformSize - 1));
    
    // Determine direction and fill all points between last and current
    int startIndex = std::min(lastIndex, currentIndex);
    int endIndex = std::max(lastIndex, currentIndex);
    
    for (int i = startIndex; i <= endIndex; ++i)
    {
        // Calculate interpolation factor
        float t = 0.0f;
        if (endIndex != startIndex)
        {
            if (currentIndex > lastIndex)
            {
                t = static_cast<float>(i - lastIndex) / static_cast<float>(currentIndex - lastIndex);
            }
            else
            {
                t = static_cast<float>(i - currentIndex) / static_cast<float>(lastIndex - currentIndex);
                t = 1.0f - t;  // Reverse interpolation direction
            }
        }
        
        // Interpolate pitch offset
        float interpolatedPitch = lastPitchOffset + t * (currentPitchOffset - lastPitchOffset);
        interpolatedPitch = juce::jlimit(-12.0f, 12.0f, interpolatedPitch);
        
        // Store the interpolated value
        int clampedIndex = juce::jlimit(0, waveformSize - 1, i);
        colorConfig.pitchWaveform[clampedIndex] = interpolatedPitch;
    }
    
    // Update last position for next drag event
    lastNormalizedX = currentNormalizedX;
    lastPitchOffset = currentPitchOffset;
    
    repaint();
}

void PitchSequencerComponent::mouseUp(const juce::MouseEvent& event)
{
    // Stop drawing
    isDrawing = false;
}

//==============================================================================
void PitchSequencerComponent::recordPitchOffset(float normalizedX, float pitchOffset)
{
    auto& colorConfig = patternModel.getColorConfig(selectedColorChannel);
    
    // Ensure waveform is initialized
    if (colorConfig.pitchWaveform.empty())
    {
        colorConfig.pitchWaveform.resize(waveformResolution, 0.0f);
    }
    
    // Map normalized X to waveform index
    int index = static_cast<int>(normalizedX * (colorConfig.pitchWaveform.size() - 1));
    index = juce::jlimit(0, static_cast<int>(colorConfig.pitchWaveform.size() - 1), index);
    
    // Store the pitch offset
    colorConfig.pitchWaveform[index] = pitchOffset;
}

void PitchSequencerComponent::initializeWaveform()
{
    // This method is now deprecated - waveforms are initialized per-color in PatternModel
    // Kept for compatibility but does nothing
}

} // namespace SquareBeats

#include "GateFlashOverlay.h"

namespace SquareBeats {

//==============================================================================
GateFlashOverlay::GateFlashOverlay(PatternModel& model, VisualFeedbackState& feedbackState)
    : patternModel(model)
    , visualFeedback(feedbackState)
{
    // This component should not intercept mouse events
    setInterceptsMouseClicks(false, false);
    
    // Make it transparent so we only see the flash effects
    setOpaque(false);
}

GateFlashOverlay::~GateFlashOverlay()
{
}

//==============================================================================
void GateFlashOverlay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Accumulate color from all active flashes
    float totalR = 0.0f, totalG = 0.0f, totalB = 0.0f, totalA = 0.0f;
    
    for (int colorId = 0; colorId < 4; ++colorId)
    {
        float intensity = visualFeedback.getFlashIntensity(colorId);
        
        if (intensity > 0.001f)
        {
            const auto& colorConfig = patternModel.getColorConfig(colorId);
            juce::Colour color = colorConfig.displayColor;
            
            // Scale intensity by velocity for dynamic response
            int velocity = visualFeedback.getVelocity(colorId);
            float velocityScale = 0.5f + 0.5f * (velocity / 127.0f);
            intensity *= velocityScale;
            
            // Additive blending
            float alpha = intensity * flashOpacity;
            totalR += color.getFloatRed() * alpha;
            totalG += color.getFloatGreen() * alpha;
            totalB += color.getFloatBlue() * alpha;
            totalA += alpha;
        }
    }
    
    // Draw the combined flash if there's any intensity
    if (totalA > 0.001f)
    {
        // Clamp values to valid range
        totalR = juce::jlimit(0.0f, 1.0f, totalR);
        totalG = juce::jlimit(0.0f, 1.0f, totalG);
        totalB = juce::jlimit(0.0f, 1.0f, totalB);
        totalA = juce::jlimit(0.0f, 0.5f, totalA);  // Cap alpha to prevent overwhelming
        
        juce::Colour flashColor = juce::Colour::fromFloatRGBA(totalR, totalG, totalB, totalA);
        
        // Draw as a gradient from center for more interesting visual
        juce::ColourGradient gradient(
            flashColor,
            bounds.getCentreX(), bounds.getCentreY(),
            flashColor.withAlpha(0.0f),
            bounds.getX(), bounds.getY(),
            true  // Radial gradient
        );
        
        g.setGradientFill(gradient);
        g.fillRect(bounds);
    }
}

} // namespace SquareBeats

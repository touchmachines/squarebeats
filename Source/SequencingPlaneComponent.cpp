#include "SequencingPlaneComponent.h"

namespace SquareBeats {

//==============================================================================
SequencingPlaneComponent::SequencingPlaneComponent(PatternModel& model)
    : patternModel(model)
    , playbackPosition(0.0f)
    , selectedColorChannel(0)
    , isCreatingSquare(false)
    , currentlyCreatingSquare(nullptr)
    , currentEditMode(EditMode::None)
    , selectedSquare(nullptr)
    , editStartLeft(0.0f)
    , editStartTop(0.0f)
    , editStartWidth(0.0f)
    , editStartHeight(0.0f)
{
    // Component will be opaque for better rendering performance
    setOpaque(true);
    
    // Enable keyboard focus for delete key
    setWantsKeyboardFocus(true);
}

SequencingPlaneComponent::~SequencingPlaneComponent()
{
}

//==============================================================================
void SequencingPlaneComponent::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    // Draw grid lines
    drawGridLines(g);
    
    // Draw all squares
    drawSquares(g);
    
    // Draw active square glow effects
    drawActiveSquareGlow(g);
    
    // Draw playback position indicator
    drawPlaybackIndicator(g);
}

void SequencingPlaneComponent::resized()
{
    // Nothing to do here for now - all rendering is done in paint()
    // using dynamic coordinate conversion
}

//==============================================================================
void SequencingPlaneComponent::setPlaybackPosition(float normalizedPosition)
{
    if (playbackPosition != normalizedPosition)
    {
        playbackPosition = normalizedPosition;
        repaint();
    }
}

void SequencingPlaneComponent::setSelectedColorChannel(int colorId)
{
    selectedColorChannel = juce::jlimit(0, 3, colorId);
}

//==============================================================================
void SequencingPlaneComponent::drawGridLines(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Get time signature and loop length for grid calculation
    auto timeSignature = patternModel.getTimeSignature();
    int loopLength = patternModel.getLoopLength();
    
    // Calculate total beats in the loop
    double beatsPerBar = timeSignature.getBeatsPerBar();
    double totalBeats = loopLength * beatsPerBar;
    
    // Calculate beat pulse brightness boost
    float pulseBoost = beatPulseIntensity * 0.3f;
    
    // Draw vertical grid lines (time divisions)
    g.setColour(juce::Colour(0xff333333));
    
    // Draw bar lines (stronger)
    for (int bar = 0; bar <= loopLength; ++bar)
    {
        float normalizedX = bar / static_cast<float>(loopLength);
        float pixelX = bounds.getX() + normalizedX * bounds.getWidth();
        
        // Bar lines pulse brighter on downbeats
        uint8_t baseGray = 0x55;
        uint8_t pulsedGray = static_cast<uint8_t>(juce::jmin(255.0f, baseGray + pulseBoost * 200.0f));
        g.setColour(juce::Colour(pulsedGray, pulsedGray, pulsedGray));
        g.drawLine(pixelX, bounds.getY(), pixelX, bounds.getBottom(), 2.0f);
    }
    
    // Draw beat lines (lighter, with subtle pulse)
    uint8_t beatGray = static_cast<uint8_t>(juce::jmin(255.0f, 0x33 + pulseBoost * 100.0f));
    g.setColour(juce::Colour(beatGray, beatGray, beatGray));
    for (int beat = 0; beat < static_cast<int>(totalBeats); ++beat)
    {
        float normalizedX = beat / static_cast<float>(totalBeats);
        float pixelX = bounds.getX() + normalizedX * bounds.getWidth();
        
        g.drawLine(pixelX, bounds.getY(), pixelX, bounds.getBottom(), 1.0f);
    }
    
    // Draw horizontal grid lines (pitch divisions)
    // Draw lines every 1/8th of the height for reference
    for (int i = 0; i <= 8; ++i)
    {
        float normalizedY = i / 8.0f;
        float pixelY = bounds.getY() + normalizedY * bounds.getHeight();
        
        g.setColour(juce::Colour(beatGray, beatGray, beatGray));
        g.drawLine(bounds.getX(), pixelY, bounds.getRight(), pixelY, 1.0f);
    }
}

void SequencingPlaneComponent::drawSquares(juce::Graphics& g)
{
    auto squares = patternModel.getAllSquares();
    
    for (const auto* square : squares)
    {
        // Get the color for this square's channel
        const auto& colorConfig = patternModel.getColorConfig(square->colorChannelId);
        
        // Convert normalized coordinates to pixel coordinates
        auto pixelRect = normalizedToPixels(
            square->leftEdge,
            square->topEdge,
            square->width,
            square->height
        );
        
        // Draw filled square with the channel's color
        g.setColour(colorConfig.displayColor.withAlpha(0.7f));
        g.fillRect(pixelRect);
        
        // Draw border
        g.setColour(colorConfig.displayColor);
        g.drawRect(pixelRect, 2.0f);
    }
}

void SequencingPlaneComponent::drawPlaybackIndicator(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Convert playback position to pixel X coordinate
    float pixelX = bounds.getX() + playbackPosition * bounds.getWidth();
    
    // Draw motion blur trail behind playhead
    float trailLength = 30.0f;
    juce::ColourGradient trailGradient(
        juce::Colours::white.withAlpha(0.0f),
        pixelX - trailLength, bounds.getCentreY(),
        juce::Colours::white.withAlpha(0.3f),
        pixelX, bounds.getCentreY(),
        false
    );
    g.setGradientFill(trailGradient);
    g.fillRect(pixelX - trailLength, bounds.getY(), trailLength, bounds.getHeight());
    
    // Draw main playhead line with glow
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawLine(pixelX - 2.0f, bounds.getY(), pixelX - 2.0f, bounds.getBottom(), 4.0f);
    
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.drawLine(pixelX, bounds.getY(), pixelX, bounds.getBottom(), 2.0f);
}

void SequencingPlaneComponent::drawActiveSquareGlow(juce::Graphics& g)
{
    if (visualFeedback == nullptr) return;
    
    auto squares = patternModel.getAllSquares();
    
    for (const auto* square : squares)
    {
        int colorId = square->colorChannelId;
        
        // Check if this color channel has an active gate
        if (visualFeedback->isGateOn(colorId))
        {
            const auto& colorConfig = patternModel.getColorConfig(colorId);
            
            // Convert normalized coordinates to pixel coordinates
            auto pixelRect = normalizedToPixels(
                square->leftEdge,
                square->topEdge,
                square->width,
                square->height
            );
            
            // Get glow intensity (pulses while note is held)
            float glowIntensity = visualFeedback->getActiveGlowIntensity(colorId);
            
            // Draw outer glow
            float glowSize = 8.0f * glowIntensity;
            auto glowRect = pixelRect.expanded(glowSize);
            
            // Create gradient for glow effect
            juce::Colour glowColor = colorConfig.displayColor.withAlpha(0.4f * glowIntensity);
            
            g.setColour(glowColor);
            g.drawRect(glowRect, 3.0f);
            
            // Draw inner highlight
            g.setColour(colorConfig.displayColor.withAlpha(0.6f * glowIntensity));
            g.drawRect(pixelRect.expanded(2.0f), 2.0f);
        }
        
        // Also draw velocity-based ripple effect on trigger
        float flashIntensity = visualFeedback->getFlashIntensity(square->colorChannelId);
        if (flashIntensity > 0.01f)
        {
            const auto& colorConfig = patternModel.getColorConfig(square->colorChannelId);
            int velocity = visualFeedback->getVelocity(square->colorChannelId);
            
            auto pixelRect = normalizedToPixels(
                square->leftEdge,
                square->topEdge,
                square->width,
                square->height
            );
            
            // Velocity affects ripple size and intensity
            float velocityScale = velocity / 127.0f;
            float maxRippleSize = 30.0f + 40.0f * velocityScale;  // 30-70px based on velocity
            
            // Draw multiple expanding ripple rings
            for (int ring = 0; ring < 3; ++ring)
            {
                // Stagger the rings with different phases
                float ringPhase = (1.0f - flashIntensity) + ring * 0.15f;
                if (ringPhase > 1.0f) continue;
                
                float rippleSize = maxRippleSize * ringPhase;
                float ringAlpha = (1.0f - ringPhase) * (0.4f - ring * 0.1f) * velocityScale;
                
                if (ringAlpha > 0.01f)
                {
                    auto rippleRect = pixelRect.expanded(rippleSize);
                    
                    // Draw rounded ripple for softer look
                    g.setColour(colorConfig.displayColor.withAlpha(ringAlpha));
                    g.drawRoundedRectangle(rippleRect, 4.0f, 2.0f - ring * 0.5f);
                }
            }
        }
    }
}

//==============================================================================
juce::Rectangle<float> SequencingPlaneComponent::normalizedToPixels(
    float left, float top, float width, float height) const
{
    auto bounds = getLocalBounds().toFloat();
    
    return juce::Rectangle<float>(
        bounds.getX() + left * bounds.getWidth(),
        bounds.getY() + top * bounds.getHeight(),
        width * bounds.getWidth(),
        height * bounds.getHeight()
    );
}

float SequencingPlaneComponent::pixelXToNormalized(float pixelX) const
{
    auto bounds = getLocalBounds().toFloat();
    return (pixelX - bounds.getX()) / bounds.getWidth();
}

float SequencingPlaneComponent::pixelYToNormalized(float pixelY) const
{
    auto bounds = getLocalBounds().toFloat();
    return (pixelY - bounds.getY()) / bounds.getHeight();
}

//==============================================================================
void SequencingPlaneComponent::mouseDown(const juce::MouseEvent& event)
{
    // Convert to normalized coordinates
    auto mousePos = event.position;
    float normalizedX = juce::jlimit(0.0f, 1.0f, pixelXToNormalized(mousePos.x));
    float normalizedY = juce::jlimit(0.0f, 1.0f, pixelYToNormalized(mousePos.y));
    
    // Check if we clicked on an existing square
    Square* clickedSquare = findSquareAt(normalizedX, normalizedY);
    
    if (clickedSquare != nullptr)
    {
        // We clicked on an existing square - determine edit mode
        selectedSquare = clickedSquare;
        currentEditMode = determineEditMode(clickedSquare, normalizedX, normalizedY);
        
        // Store initial state for editing
        editStartMousePos = juce::Point<float>(normalizedX, normalizedY);
        editStartLeft = clickedSquare->leftEdge;
        editStartTop = clickedSquare->topEdge;
        editStartWidth = clickedSquare->width;
        editStartHeight = clickedSquare->height;
        
        // Right-click deletes the square
        if (event.mods.isPopupMenu())
        {
            patternModel.deleteSquare(clickedSquare->uniqueId);
            selectedSquare = nullptr;
            currentEditMode = EditMode::None;
            repaint();
        }
    }
    else
    {
        // Start creating a new square
        dragStartPoint = juce::Point<float>(normalizedX, normalizedY);
        currentEditMode = EditMode::Creating;
        isCreatingSquare = true;
        currentlyCreatingSquare = nullptr;
        selectedSquare = nullptr;
    }
}

void SequencingPlaneComponent::mouseDrag(const juce::MouseEvent& event)
{
    auto mousePos = event.position;
    float normalizedX = juce::jlimit(0.0f, 1.0f, pixelXToNormalized(mousePos.x));
    float normalizedY = juce::jlimit(0.0f, 1.0f, pixelYToNormalized(mousePos.y));
    
    const float minSize = 0.01f;
    
    if (currentEditMode == EditMode::Creating)
    {
        // Creating a new square
        float left = juce::jmin(dragStartPoint.x, normalizedX);
        float top = juce::jmin(dragStartPoint.y, normalizedY);
        float right = juce::jmax(dragStartPoint.x, normalizedX);
        float bottom = juce::jmax(dragStartPoint.y, normalizedY);
        
        float width = juce::jmax(minSize, right - left);
        float height = juce::jmax(minSize, bottom - top);
        
        if (currentlyCreatingSquare == nullptr)
        {
            currentlyCreatingSquare = patternModel.createSquare(
                left, top, width, height, selectedColorChannel
            );
        }
        else
        {
            patternModel.moveSquare(currentlyCreatingSquare->uniqueId, left, top);
            patternModel.resizeSquare(currentlyCreatingSquare->uniqueId, width, height);
        }
        
        repaint();
    }
    else if (selectedSquare != nullptr && currentEditMode != EditMode::None)
    {
        // Editing an existing square
        float deltaX = normalizedX - editStartMousePos.x;
        float deltaY = normalizedY - editStartMousePos.y;
        
        float newLeft = editStartLeft;
        float newTop = editStartTop;
        float newWidth = editStartWidth;
        float newHeight = editStartHeight;
        
        switch (currentEditMode)
        {
            case EditMode::Moving:
                newLeft = juce::jlimit(0.0f, 1.0f - editStartWidth, editStartLeft + deltaX);
                newTop = juce::jlimit(0.0f, 1.0f - editStartHeight, editStartTop + deltaY);
                break;
                
            case EditMode::ResizingLeft:
                newLeft = juce::jlimit(0.0f, editStartLeft + editStartWidth - minSize, editStartLeft + deltaX);
                newWidth = editStartLeft + editStartWidth - newLeft;
                break;
                
            case EditMode::ResizingRight:
                newWidth = juce::jlimit(minSize, 1.0f - editStartLeft, editStartWidth + deltaX);
                break;
                
            case EditMode::ResizingTop:
                newTop = juce::jlimit(0.0f, editStartTop + editStartHeight - minSize, editStartTop + deltaY);
                newHeight = editStartTop + editStartHeight - newTop;
                break;
                
            case EditMode::ResizingBottom:
                newHeight = juce::jlimit(minSize, 1.0f - editStartTop, editStartHeight + deltaY);
                break;
                
            case EditMode::ResizingTopLeft:
                newLeft = juce::jlimit(0.0f, editStartLeft + editStartWidth - minSize, editStartLeft + deltaX);
                newTop = juce::jlimit(0.0f, editStartTop + editStartHeight - minSize, editStartTop + deltaY);
                newWidth = editStartLeft + editStartWidth - newLeft;
                newHeight = editStartTop + editStartHeight - newTop;
                break;
                
            case EditMode::ResizingTopRight:
                newTop = juce::jlimit(0.0f, editStartTop + editStartHeight - minSize, editStartTop + deltaY);
                newWidth = juce::jlimit(minSize, 1.0f - editStartLeft, editStartWidth + deltaX);
                newHeight = editStartTop + editStartHeight - newTop;
                break;
                
            case EditMode::ResizingBottomLeft:
                newLeft = juce::jlimit(0.0f, editStartLeft + editStartWidth - minSize, editStartLeft + deltaX);
                newWidth = editStartLeft + editStartWidth - newLeft;
                newHeight = juce::jlimit(minSize, 1.0f - editStartTop, editStartHeight + deltaY);
                break;
                
            case EditMode::ResizingBottomRight:
                newWidth = juce::jlimit(minSize, 1.0f - editStartLeft, editStartWidth + deltaX);
                newHeight = juce::jlimit(minSize, 1.0f - editStartTop, editStartHeight + deltaY);
                break;
                
            default:
                break;
        }
        
        // Apply the changes
        patternModel.moveSquare(selectedSquare->uniqueId, newLeft, newTop);
        patternModel.resizeSquare(selectedSquare->uniqueId, newWidth, newHeight);
        
        repaint();
    }
}

void SequencingPlaneComponent::mouseUp(const juce::MouseEvent& event)
{
    if (currentEditMode == EditMode::Creating)
    {
        // If we created a square during the drag, it's already in the model
        // If the drag was too small and no square was created, create a minimum-size square
        if (currentlyCreatingSquare == nullptr)
        {
            auto mousePos = event.position;
            float normalizedX = juce::jlimit(0.0f, 1.0f, pixelXToNormalized(mousePos.x));
            float normalizedY = juce::jlimit(0.0f, 1.0f, pixelYToNormalized(mousePos.y));
            
            const float minSize = 0.01f;
            patternModel.createSquare(
                normalizedX, normalizedY, minSize, minSize, selectedColorChannel
            );
            
            repaint();
        }
        
        isCreatingSquare = false;
        currentlyCreatingSquare = nullptr;
    }
    
    // Reset edit state
    currentEditMode = EditMode::None;
    selectedSquare = nullptr;
}

void SequencingPlaneComponent::mouseDoubleClick(const juce::MouseEvent& event)
{
    // Convert to normalized coordinates
    auto mousePos = event.position;
    float normalizedX = juce::jlimit(0.0f, 1.0f, pixelXToNormalized(mousePos.x));
    float normalizedY = juce::jlimit(0.0f, 1.0f, pixelYToNormalized(mousePos.y));
    
    // Check if we double-clicked on an existing square
    Square* clickedSquare = findSquareAt(normalizedX, normalizedY);
    
    if (clickedSquare != nullptr)
    {
        // Delete the square
        patternModel.deleteSquare(clickedSquare->uniqueId);
        selectedSquare = nullptr;
        currentEditMode = EditMode::None;
        repaint();
    }
}

bool SequencingPlaneComponent::keyPressed(const juce::KeyPress& key)
{
    // Delete key removes the selected square
    if (key == juce::KeyPress::deleteKey || key == juce::KeyPress::backspaceKey)
    {
        if (selectedSquare != nullptr)
        {
            patternModel.deleteSquare(selectedSquare->uniqueId);
            selectedSquare = nullptr;
            repaint();
            return true;
        }
    }
    
    return false;
}

//==============================================================================
Square* SequencingPlaneComponent::findSquareAt(float normalizedX, float normalizedY)
{
    auto squares = patternModel.getAllSquares();
    
    // Search in reverse order so we find the topmost (most recently drawn) square
    for (auto it = squares.rbegin(); it != squares.rend(); ++it)
    {
        Square* square = *it;
        
        if (normalizedX >= square->leftEdge && 
            normalizedX <= square->leftEdge + square->width &&
            normalizedY >= square->topEdge && 
            normalizedY <= square->topEdge + square->height)
        {
            return square;
        }
    }
    
    return nullptr;
}

SequencingPlaneComponent::EditMode SequencingPlaneComponent::determineEditMode(
    const Square* square, float normalizedX, float normalizedY) const
{
    if (square == nullptr)
        return EditMode::None;
    
    // Define edge detection threshold (in normalized coordinates)
    const float edgeThreshold = 0.02f;
    
    // Check which edges we're near
    bool nearLeft = (normalizedX - square->leftEdge) < edgeThreshold;
    bool nearRight = (square->leftEdge + square->width - normalizedX) < edgeThreshold;
    bool nearTop = (normalizedY - square->topEdge) < edgeThreshold;
    bool nearBottom = (square->topEdge + square->height - normalizedY) < edgeThreshold;
    
    // Determine edit mode based on which edges we're near
    if (nearTop && nearLeft)
        return EditMode::ResizingTopLeft;
    else if (nearTop && nearRight)
        return EditMode::ResizingTopRight;
    else if (nearBottom && nearLeft)
        return EditMode::ResizingBottomLeft;
    else if (nearBottom && nearRight)
        return EditMode::ResizingBottomRight;
    else if (nearLeft)
        return EditMode::ResizingLeft;
    else if (nearRight)
        return EditMode::ResizingRight;
    else if (nearTop)
        return EditMode::ResizingTop;
    else if (nearBottom)
        return EditMode::ResizingBottom;
    else
        return EditMode::Moving;
}

} // namespace SquareBeats

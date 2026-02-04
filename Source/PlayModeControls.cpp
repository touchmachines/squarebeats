#include "PlayModeControls.h"

namespace SquareBeats {

//==============================================================================
// XYPadComponent Implementation
//==============================================================================

XYPadComponent::XYPadComponent()
{
}

void XYPadComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Background
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(bounds, 4.0f);
    
    // Border
    g.setColour(juce::Colour(0xff555555));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 4.0f, 2.0f);
    
    // Grid lines
    g.setColour(juce::Colour(0xff333333));
    float centerX = bounds.getCentreX();
    float centerY = bounds.getCentreY();
    g.drawLine(centerX, bounds.getY() + 4, centerX, bounds.getBottom() - 4, 1.0f);
    g.drawLine(bounds.getX() + 4, centerY, bounds.getRight() - 4, centerY, 1.0f);
    
    // Calculate cursor position
    float cursorX = bounds.getX() + xValue * bounds.getWidth();
    float cursorY = bounds.getBottom() - yValue * bounds.getHeight();  // Y inverted (0 at bottom)
    
    // Draw crosshairs
    g.setColour(juce::Colour(0xff888888));
    g.drawLine(cursorX, bounds.getY() + 2, cursorX, bounds.getBottom() - 2, 1.0f);
    g.drawLine(bounds.getX() + 2, cursorY, bounds.getRight() - 2, cursorY, 1.0f);
    
    // Draw cursor dot
    g.setColour(juce::Colours::white);
    g.fillEllipse(cursorX - 6.0f, cursorY - 6.0f, 12.0f, 12.0f);
    g.setColour(juce::Colour(0xff00aaff));
    g.fillEllipse(cursorX - 4.0f, cursorY - 4.0f, 8.0f, 8.0f);
}

void XYPadComponent::mouseDown(const juce::MouseEvent& event)
{
    updateFromMouse(event.position);
}

void XYPadComponent::mouseDrag(const juce::MouseEvent& event)
{
    updateFromMouse(event.position);
}

void XYPadComponent::updateFromMouse(const juce::Point<float>& pos)
{
    auto bounds = getLocalBounds().toFloat();
    
    float newX = (pos.x - bounds.getX()) / bounds.getWidth();
    float newY = 1.0f - (pos.y - bounds.getY()) / bounds.getHeight();  // Y inverted
    
    xValue = juce::jlimit(0.0f, 1.0f, newX);
    yValue = juce::jlimit(0.0f, 1.0f, newY);
    
    repaint();
    
    if (onValueChanged)
    {
        onValueChanged(xValue, yValue);
    }
}

//==============================================================================
// PlayModeButtons Implementation
//==============================================================================

PlayModeButtons::PlayModeButtons(PatternModel& model)
    : patternModel(model)
{
    setupComponents();
    refreshFromModel();
}

PlayModeButtons::~PlayModeButtons()
{
}

void PlayModeButtons::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
}

void PlayModeButtons::resized()
{
    auto bounds = getLocalBounds().reduced(2);
    int buttonWidth = bounds.getWidth() / 4;
    
    forwardButton.setBounds(bounds.removeFromLeft(buttonWidth).reduced(2, 0));
    backwardButton.setBounds(bounds.removeFromLeft(buttonWidth).reduced(2, 0));
    pendulumButton.setBounds(bounds.removeFromLeft(buttonWidth).reduced(2, 0));
    probabilityButton.setBounds(bounds.reduced(2, 0));
}

void PlayModeButtons::refreshFromModel()
{
    updateButtonStates();
}

void PlayModeButtons::setupComponents()
{
    forwardButton.setButtonText("-->");
    forwardButton.onClick = [this]() { onModeButtonClicked(PLAY_FORWARD); };
    addAndMakeVisible(forwardButton);
    
    backwardButton.setButtonText("<--");
    backwardButton.onClick = [this]() { onModeButtonClicked(PLAY_BACKWARD); };
    addAndMakeVisible(backwardButton);
    
    pendulumButton.setButtonText("<-->");
    pendulumButton.onClick = [this]() { onModeButtonClicked(PLAY_PENDULUM); };
    addAndMakeVisible(pendulumButton);
    
    probabilityButton.setButtonText("--?>");
    probabilityButton.onClick = [this]() { onModeButtonClicked(PLAY_PROBABILITY); };
    addAndMakeVisible(probabilityButton);
}

void PlayModeButtons::onModeButtonClicked(PlayMode mode)
{
    auto& config = patternModel.getPlayModeConfig();
    bool wasProbability = (config.mode == PLAY_PROBABILITY);
    
    config.mode = mode;
    config.pendulumForward = true;
    
    updateButtonStates();
    patternModel.sendChangeMessage();
    
    // Notify if probability mode changed
    bool isProbability = (mode == PLAY_PROBABILITY);
    if (onProbabilityModeChanged && wasProbability != isProbability)
    {
        onProbabilityModeChanged(isProbability);
    }
}

void PlayModeButtons::updateButtonStates()
{
    auto& config = patternModel.getPlayModeConfig();
    
    juce::Colour activeColour(0xff4488ff);
    juce::Colour inactiveColour = getLookAndFeel().findColour(juce::TextButton::buttonColourId);
    
    forwardButton.setColour(juce::TextButton::buttonColourId, 
        config.mode == PLAY_FORWARD ? activeColour : inactiveColour);
    backwardButton.setColour(juce::TextButton::buttonColourId, 
        config.mode == PLAY_BACKWARD ? activeColour : inactiveColour);
    pendulumButton.setColour(juce::TextButton::buttonColourId, 
        config.mode == PLAY_PENDULUM ? activeColour : inactiveColour);
    probabilityButton.setColour(juce::TextButton::buttonColourId, 
        config.mode == PLAY_PROBABILITY ? activeColour : inactiveColour);
}

//==============================================================================
// PlayModeXYPad Implementation
//==============================================================================

PlayModeXYPad::PlayModeXYPad(PatternModel& model)
    : patternModel(model)
{
    setupComponents();
    refreshFromModel();
}

PlayModeXYPad::~PlayModeXYPad()
{
}

void PlayModeXYPad::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
}

void PlayModeXYPad::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    
    // Label at top
    xyPadLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(2);
    
    // Y-axis label on left
    auto xyArea = bounds.removeFromTop(140);
    yAxisLabel.setBounds(xyArea.removeFromLeft(20));
    
    // XY Pad
    xyPad->setBounds(xyArea.reduced(2));
    
    bounds.removeFromTop(2);
    
    // X-axis label below
    xAxisLabel.setBounds(bounds.removeFromTop(16).withTrimmedLeft(20));
}

void PlayModeXYPad::refreshFromModel()
{
    auto& config = patternModel.getPlayModeConfig();
    xyPad->setXValue(config.stepJumpSize);
    xyPad->setYValue(config.probability);
}

void PlayModeXYPad::setupComponents()
{
    xyPad = std::make_unique<XYPadComponent>();
    xyPad->onValueChanged = [this](float x, float y) { onXYPadChanged(x, y); };
    addAndMakeVisible(xyPad.get());
    
    xyPadLabel.setText("PROBABILITY XY PAD", juce::dontSendNotification);
    xyPadLabel.setJustificationType(juce::Justification::centred);
    xyPadLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(xyPadLabel);
    
    xAxisLabel.setText("Step Jump Size", juce::dontSendNotification);
    xAxisLabel.setJustificationType(juce::Justification::centred);
    xAxisLabel.setFont(juce::Font(10.0f));
    addAndMakeVisible(xAxisLabel);
    
    yAxisLabel.setText("Prob%", juce::dontSendNotification);
    yAxisLabel.setJustificationType(juce::Justification::centred);
    yAxisLabel.setFont(juce::Font(9.0f));
    addAndMakeVisible(yAxisLabel);
}

void PlayModeXYPad::onXYPadChanged(float x, float y)
{
    auto& config = patternModel.getPlayModeConfig();
    config.stepJumpSize = x;
    config.probability = y;
}

//==============================================================================
// PlayModeControls Implementation (Legacy - kept for compatibility)
//==============================================================================

PlayModeControls::PlayModeControls(PatternModel& model)
    : patternModel(model)
{
    setupComponents();
    refreshFromModel();
}

PlayModeControls::~PlayModeControls()
{
}

void PlayModeControls::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
}

void PlayModeControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    
    // Mode buttons in a row
    auto buttonRow = bounds.removeFromTop(30);
    int buttonWidth = buttonRow.getWidth() / 4;
    
    forwardButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2, 0));
    backwardButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2, 0));
    pendulumButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2, 0));
    probabilityButton.setBounds(buttonRow.reduced(2, 0));
    
    bounds.removeFromTop(5);
    
    // XY Pad area (only visible in probability mode)
    if (xyPad->isVisible())
    {
        // Label
        xyPadLabel.setBounds(bounds.removeFromTop(20));
        bounds.removeFromTop(2);
        
        // XY Pad with axis labels
        auto xyArea = bounds.removeFromTop(120);
        
        // Y-axis label on left
        yAxisLabel.setBounds(xyArea.removeFromLeft(20));
        
        // XY Pad
        xyPad->setBounds(xyArea.removeFromLeft(xyArea.getWidth() - 5));
        
        bounds.removeFromTop(2);
        
        // X-axis label below
        xAxisLabel.setBounds(bounds.removeFromTop(16).withTrimmedLeft(20));
    }
}

void PlayModeControls::refreshFromModel()
{
    auto& config = patternModel.getPlayModeConfig();
    
    updateButtonStates();
    
    // Update XY pad values
    xyPad->setXValue(config.stepJumpSize);
    xyPad->setYValue(config.probability);
    
    // Show/hide XY pad based on mode
    bool showXYPad = (config.mode == PLAY_PROBABILITY);
    xyPad->setVisible(showXYPad);
    xyPadLabel.setVisible(showXYPad);
    xAxisLabel.setVisible(showXYPad);
    yAxisLabel.setVisible(showXYPad);
    
    resized();
    repaint();
}

void PlayModeControls::setupComponents()
{
    // Forward button
    forwardButton.setButtonText("-->");
    forwardButton.onClick = [this]() { onModeButtonClicked(PLAY_FORWARD); };
    addAndMakeVisible(forwardButton);
    
    // Backward button
    backwardButton.setButtonText("<--");
    backwardButton.onClick = [this]() { onModeButtonClicked(PLAY_BACKWARD); };
    addAndMakeVisible(backwardButton);
    
    // Pendulum button
    pendulumButton.setButtonText("<-->");
    pendulumButton.onClick = [this]() { onModeButtonClicked(PLAY_PENDULUM); };
    addAndMakeVisible(pendulumButton);
    
    // Probability button
    probabilityButton.setButtonText("--?>");
    probabilityButton.onClick = [this]() { onModeButtonClicked(PLAY_PROBABILITY); };
    addAndMakeVisible(probabilityButton);
    
    // XY Pad
    xyPad = std::make_unique<XYPadComponent>();
    xyPad->onValueChanged = [this](float x, float y) { onXYPadChanged(x, y); };
    addAndMakeVisible(xyPad.get());
    
    // XY Pad label
    xyPadLabel.setText("PROBABILITY", juce::dontSendNotification);
    xyPadLabel.setJustificationType(juce::Justification::centred);
    xyPadLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(xyPadLabel);
    
    // X-axis label
    xAxisLabel.setText("Step Jump Size", juce::dontSendNotification);
    xAxisLabel.setJustificationType(juce::Justification::centred);
    xAxisLabel.setFont(juce::Font(10.0f));
    addAndMakeVisible(xAxisLabel);
    
    // Y-axis label (rotated text would be ideal, but we'll use abbreviated)
    yAxisLabel.setText("Prob%", juce::dontSendNotification);
    yAxisLabel.setJustificationType(juce::Justification::centred);
    yAxisLabel.setFont(juce::Font(9.0f));
    addAndMakeVisible(yAxisLabel);
}

void PlayModeControls::onModeButtonClicked(PlayMode mode)
{
    auto& config = patternModel.getPlayModeConfig();
    config.mode = mode;
    
    // Reset pendulum direction when switching modes
    config.pendulumForward = true;
    
    refreshFromModel();
    patternModel.sendChangeMessage();
}

void PlayModeControls::updateButtonStates()
{
    auto& config = patternModel.getPlayModeConfig();
    
    // Highlight the active mode button
    juce::Colour activeColour(0xff4488ff);
    juce::Colour inactiveColour = getLookAndFeel().findColour(juce::TextButton::buttonColourId);
    
    forwardButton.setColour(juce::TextButton::buttonColourId, 
        config.mode == PLAY_FORWARD ? activeColour : inactiveColour);
    backwardButton.setColour(juce::TextButton::buttonColourId, 
        config.mode == PLAY_BACKWARD ? activeColour : inactiveColour);
    pendulumButton.setColour(juce::TextButton::buttonColourId, 
        config.mode == PLAY_PENDULUM ? activeColour : inactiveColour);
    probabilityButton.setColour(juce::TextButton::buttonColourId, 
        config.mode == PLAY_PROBABILITY ? activeColour : inactiveColour);
}

void PlayModeControls::onXYPadChanged(float x, float y)
{
    auto& config = patternModel.getPlayModeConfig();
    config.stepJumpSize = x;
    config.probability = y;
    
    // Don't send change message for every drag - too noisy
    // The values are read directly during playback
}

} // namespace SquareBeats

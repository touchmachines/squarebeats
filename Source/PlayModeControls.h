#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * XYPadComponent - A 2D controller for step jump size (X) and probability (Y)
 */
class XYPadComponent : public juce::Component
{
public:
    XYPadComponent();
    ~XYPadComponent() override = default;
    
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    
    // Get/set normalized values (0.0 to 1.0)
    float getXValue() const { return xValue; }
    float getYValue() const { return yValue; }
    void setXValue(float x) { xValue = juce::jlimit(0.0f, 1.0f, x); repaint(); }
    void setYValue(float y) { yValue = juce::jlimit(0.0f, 1.0f, y); repaint(); }
    
    std::function<void(float, float)> onValueChanged;
    
private:
    float xValue = 0.5f;
    float yValue = 0.5f;
    
    void updateFromMouse(const juce::Point<float>& pos);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYPadComponent)
};

//==============================================================================
/**
 * PlayModeButtons - Compact play mode selector buttons for the top bar
 * 
 * Provides 4 play mode buttons: Forward, Backward, Pendulum, Probability
 * Designed to be placed in the top bar alongside other controls.
 */
class PlayModeButtons : public juce::Component
{
public:
    PlayModeButtons(PatternModel& model);
    ~PlayModeButtons() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void refreshFromModel();
    
    // Callback when probability mode is selected (to show/hide XY pad)
    std::function<void(bool)> onProbabilityModeChanged;

private:
    PatternModel& patternModel;
    
    juce::TextButton forwardButton;
    juce::TextButton backwardButton;
    juce::TextButton pendulumButton;
    juce::TextButton probabilityButton;
    
    void setupComponents();
    void onModeButtonClicked(PlayMode mode);
    void updateButtonStates();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayModeButtons)
};

//==============================================================================
/**
 * PlayModeXYPad - XY pad panel for probability mode parameters
 * 
 * Shows the XY pad for controlling step jump size and probability.
 * Should only be visible when probability mode is active.
 */
class PlayModeXYPad : public juce::Component
{
public:
    PlayModeXYPad(PatternModel& model);
    ~PlayModeXYPad() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void refreshFromModel();

private:
    PatternModel& patternModel;
    
    std::unique_ptr<XYPadComponent> xyPad;
    juce::Label xyPadLabel;
    juce::Label xAxisLabel;
    juce::Label yAxisLabel;
    
    void setupComponents();
    void onXYPadChanged(float x, float y);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayModeXYPad)
};

//==============================================================================
/**
 * PlayModeControls - Legacy combined component (kept for compatibility)
 * 
 * Now delegates to PlayModeButtons and PlayModeXYPad internally.
 */
class PlayModeControls : public juce::Component
{
public:
    PlayModeControls(PatternModel& model);
    ~PlayModeControls() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void refreshFromModel();

private:
    PatternModel& patternModel;
    
    // Mode buttons
    juce::TextButton forwardButton;
    juce::TextButton backwardButton;
    juce::TextButton pendulumButton;
    juce::TextButton probabilityButton;
    
    // XY Pad for probability mode
    std::unique_ptr<XYPadComponent> xyPad;
    juce::Label xyPadLabel;
    juce::Label xAxisLabel;
    juce::Label yAxisLabel;
    
    void setupComponents();
    void onModeButtonClicked(PlayMode mode);
    void updateButtonStates();
    void onXYPadChanged(float x, float y);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayModeControls)
};

} // namespace SquareBeats

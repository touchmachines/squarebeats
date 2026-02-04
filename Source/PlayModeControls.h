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
 * PlayModeControls - Play mode selector with XY pad for probability mode
 * 
 * Provides:
 * - 4 play mode buttons: Forward, Backward, Pendulum, Probability
 * - XY pad (visible only in probability mode) for step jump size and probability
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

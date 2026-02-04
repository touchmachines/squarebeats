#include "LoopLengthSelector.h"

namespace SquareBeats {

//==============================================================================
LoopLengthSelector::LoopLengthSelector(PatternModel& model)
    : patternModel(model)
{
    // Setup label
    loopLengthLabel.setText("Loop Length:", juce::dontSendNotification);
    loopLengthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    loopLengthLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(loopLengthLabel);
    
    // Setup combo box with 1-64 bars
    for (int i = 1; i <= 64; ++i)
    {
        juce::String label = (i == 1) ? "1 Bar" : juce::String(i) + " Bars";
        loopLengthCombo.addItem(label, i);
    }
    loopLengthCombo.setSelectedId(1); // Default to 1 bar
    loopLengthCombo.onChange = [this]() { onLoopLengthChanged(); };
    addAndMakeVisible(loopLengthCombo);
    
    refreshFromModel();
}

LoopLengthSelector::~LoopLengthSelector()
{
}

//==============================================================================
void LoopLengthSelector::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
}

void LoopLengthSelector::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    
    // Label on left, combo on right
    loopLengthLabel.setBounds(bounds.removeFromLeft(90));
    bounds.removeFromLeft(5);
    loopLengthCombo.setBounds(bounds);
}

//==============================================================================
void LoopLengthSelector::refreshFromModel()
{
    int currentLoopLength = patternModel.getLoopLength();
    // Clamp to valid range
    currentLoopLength = juce::jlimit(1, 64, currentLoopLength);
    loopLengthCombo.setSelectedId(currentLoopLength, juce::dontSendNotification);
}

//==============================================================================
void LoopLengthSelector::onLoopLengthChanged()
{
    int newLoopLength = loopLengthCombo.getSelectedId();
    if (newLoopLength >= 1 && newLoopLength <= 64)
    {
        patternModel.setLoopLength(newLoopLength);
    }
}

} // namespace SquareBeats

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
    
    // Setup combo box with steps (1-15) and bars (1-8, 16, 32, 64)
    int itemId = 1;
    
    // Add 1-15 steps
    for (int i = 1; i <= 15; ++i)
    {
        juce::String label = (i == 1) ? "1 Step" : juce::String(i) + " Steps";
        loopLengthCombo.addItem(label, itemId++);
    }
    
    // Add 1-8 bars
    for (int i = 1; i <= 8; ++i)
    {
        juce::String label = (i == 1) ? "1 Bar" : juce::String(i) + " Bars";
        loopLengthCombo.addItem(label, itemId++);
    }
    
    // Add 16, 32, 64 bars
    loopLengthCombo.addItem("16 Bars", itemId++);
    loopLengthCombo.addItem("32 Bars", itemId++);
    loopLengthCombo.addItem("64 Bars", itemId++);
    
    loopLengthCombo.setSelectedId(16); // Default to 1 bar (itemId 16)
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
    double currentLoopLength = patternModel.getLoopLength();
    int itemId = loopLengthToItemId(currentLoopLength);
    loopLengthCombo.setSelectedId(itemId, juce::dontSendNotification);
}

//==============================================================================
void LoopLengthSelector::onLoopLengthChanged()
{
    int selectedId = loopLengthCombo.getSelectedId();
    double newLoopLength = itemIdToLoopLength(selectedId);
    patternModel.setLoopLength(newLoopLength);
}

//==============================================================================
double LoopLengthSelector::itemIdToLoopLength(int itemId) const
{
    // Item IDs 1-15: steps (1/16 bar each)
    if (itemId >= 1 && itemId <= 15)
    {
        return itemId / 16.0;  // 1 step = 1/16 bar
    }
    
    // Item IDs 16-23: 1-8 bars
    if (itemId >= 16 && itemId <= 23)
    {
        return static_cast<double>(itemId - 15);  // 16->1, 17->2, ..., 23->8
    }
    
    // Item IDs 24-26: 16, 32, 64 bars
    if (itemId == 24) return 16.0;
    if (itemId == 25) return 32.0;
    if (itemId == 26) return 64.0;
    
    // Default to 1 bar
    return 1.0;
}

int LoopLengthSelector::loopLengthToItemId(double loopLengthBars) const
{
    // Check for steps (less than 1 bar)
    if (loopLengthBars < 1.0)
    {
        int steps = static_cast<int>(std::round(loopLengthBars * 16.0));
        steps = juce::jlimit(1, 15, steps);
        return steps;  // Item IDs 1-15
    }
    
    // Check for large values
    if (loopLengthBars >= 48.0) return 26;  // 64 bars
    if (loopLengthBars >= 24.0) return 25;  // 32 bars
    if (loopLengthBars >= 12.0) return 24;  // 16 bars
    
    // Regular bars (1-8)
    int bars = static_cast<int>(std::round(loopLengthBars));
    bars = juce::jlimit(1, 8, bars);
    return 15 + bars;  // Item IDs 16-23
}

} // namespace SquareBeats

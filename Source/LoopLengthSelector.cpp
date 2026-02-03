#include "LoopLengthSelector.h"

namespace SquareBeats {

//==============================================================================
LoopLengthSelector::LoopLengthSelector(PatternModel& model)
    : patternModel(model)
{
}

LoopLengthSelector::~LoopLengthSelector()
{
}

//==============================================================================
void LoopLengthSelector::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Draw background
    g.fillAll(juce::Colour(0xff2a2a2a));
    
    int currentLoopLength = patternModel.getLoopLength();
    
    // Draw each button
    for (int i = 0; i < NUM_LOOP_OPTIONS; ++i)
    {
        auto buttonBounds = getButtonBounds(i);
        int loopLength = LOOP_OPTIONS[i];
        
        // Check if this is the selected loop length
        bool isSelected = (loopLength == currentLoopLength);
        
        // Fill button
        if (isSelected)
        {
            g.setColour(juce::Colour(0xff4a4a4a));
        }
        else
        {
            g.setColour(juce::Colour(0xff333333));
        }
        g.fillRect(buttonBounds.reduced(2));
        
        // Draw border
        if (isSelected)
        {
            g.setColour(juce::Colours::white);
            g.drawRect(buttonBounds.reduced(2), 2);
        }
        else
        {
            g.setColour(juce::Colour(0xff444444));
            g.drawRect(buttonBounds.reduced(2), 1);
        }
        
        // Draw text
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        juce::String text = juce::String(loopLength) + (loopLength == 1 ? " Bar" : " Bars");
        g.drawText(text, buttonBounds.reduced(2), juce::Justification::centred);
    }
}

void LoopLengthSelector::resized()
{
    // Nothing to do - button bounds are calculated dynamically in paint()
}

void LoopLengthSelector::mouseDown(const juce::MouseEvent& event)
{
    int clickedButton = findButtonAt(event.getPosition());
    
    if (clickedButton >= 0 && clickedButton < NUM_LOOP_OPTIONS)
    {
        int newLoopLength = LOOP_OPTIONS[clickedButton];
        patternModel.setLoopLength(newLoopLength);
        repaint();
    }
}

//==============================================================================
void LoopLengthSelector::refreshFromModel()
{
    repaint();
}

//==============================================================================
juce::Rectangle<int> LoopLengthSelector::getButtonBounds(int buttonIndex) const
{
    auto bounds = getLocalBounds();
    int buttonWidth = bounds.getWidth() / NUM_LOOP_OPTIONS;
    
    return juce::Rectangle<int>(
        buttonIndex * buttonWidth,
        0,
        buttonWidth,
        bounds.getHeight()
    );
}

int LoopLengthSelector::findButtonAt(juce::Point<int> position) const
{
    for (int i = 0; i < NUM_LOOP_OPTIONS; ++i)
    {
        if (getButtonBounds(i).contains(position))
        {
            return i;
        }
    }
    
    return -1;
}

} // namespace SquareBeats

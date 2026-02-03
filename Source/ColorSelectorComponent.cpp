#include "ColorSelectorComponent.h"

namespace SquareBeats {

//==============================================================================
ColorSelectorComponent::ColorSelectorComponent(PatternModel& model)
    : patternModel(model)
    , selectedColorChannel(0)
{
}

ColorSelectorComponent::~ColorSelectorComponent()
{
}

//==============================================================================
void ColorSelectorComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Draw background
    g.fillAll(juce::Colour(0xff2a2a2a));
    
    // Draw each color button
    for (int i = 0; i < 4; ++i)
    {
        auto buttonBounds = getColorButtonBounds(i);
        const auto& colorConfig = patternModel.getColorConfig(i);
        
        // Fill with the color
        g.setColour(colorConfig.displayColor);
        g.fillRect(buttonBounds.reduced(2));
        
        // Draw highlight border if selected
        if (i == selectedColorChannel)
        {
            g.setColour(juce::Colours::white);
            g.drawRect(buttonBounds, 3);
        }
        else
        {
            g.setColour(juce::Colour(0xff444444));
            g.drawRect(buttonBounds, 1);
        }
    }
}

void ColorSelectorComponent::resized()
{
    // Nothing to do - button bounds are calculated dynamically in paint()
}

void ColorSelectorComponent::mouseDown(const juce::MouseEvent& event)
{
    int clickedButton = findColorButtonAt(event.getPosition());
    
    if (clickedButton >= 0 && clickedButton < 4)
    {
        setSelectedColorChannel(clickedButton);
        
        // Notify listeners
        listeners.call([clickedButton](Listener& l) {
            l.colorChannelSelected(clickedButton);
        });
    }
}

//==============================================================================
void ColorSelectorComponent::setSelectedColorChannel(int colorChannelId)
{
    if (colorChannelId >= 0 && colorChannelId < 4 && colorChannelId != selectedColorChannel)
    {
        selectedColorChannel = colorChannelId;
        repaint();
    }
}

void ColorSelectorComponent::addListener(Listener* listener)
{
    listeners.add(listener);
}

void ColorSelectorComponent::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

//==============================================================================
juce::Rectangle<int> ColorSelectorComponent::getColorButtonBounds(int colorChannelId) const
{
    auto bounds = getLocalBounds();
    int buttonWidth = bounds.getWidth() / 4;
    
    return juce::Rectangle<int>(
        colorChannelId * buttonWidth,
        0,
        buttonWidth,
        bounds.getHeight()
    );
}

int ColorSelectorComponent::findColorButtonAt(juce::Point<int> position) const
{
    for (int i = 0; i < 4; ++i)
    {
        if (getColorButtonBounds(i).contains(position))
        {
            return i;
        }
    }
    
    return -1;
}

} // namespace SquareBeats

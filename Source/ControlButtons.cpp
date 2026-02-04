#include "ControlButtons.h"

namespace SquareBeats {

//==============================================================================
ControlButtons::ControlButtons(PatternModel& model)
    : patternModel(model)
    , selectedColorChannel(0)
{
    // Controls have been moved to ColorConfigPanel
}

ControlButtons::~ControlButtons()
{
}

//==============================================================================
void ControlButtons::paint(juce::Graphics& g)
{
    // Empty - no visible controls
}

void ControlButtons::resized()
{
    // Empty - no visible controls
}

//==============================================================================
void ControlButtons::setSelectedColorChannel(int colorChannelId)
{
    if (colorChannelId >= 0 && colorChannelId < 4)
    {
        selectedColorChannel = colorChannelId;
    }
}

void ControlButtons::refreshFromModel()
{
    // Nothing to refresh - controls moved to ColorConfigPanel
}

//==============================================================================
void ControlButtons::addListener(Listener* listener)
{
    if (listener != nullptr)
    {
        listeners.push_back(listener);
    }
}

void ControlButtons::removeListener(Listener* listener)
{
    listeners.erase(
        std::remove(listeners.begin(), listeners.end(), listener),
        listeners.end()
    );
}

} // namespace SquareBeats

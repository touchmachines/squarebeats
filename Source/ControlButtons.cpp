#include "ControlButtons.h"

namespace SquareBeats {

//==============================================================================
ControlButtons::ControlButtons(PatternModel& model)
    : patternModel(model)
    , selectedColorChannel(0)
{
    setupComponents();
    refreshFromModel();
}

ControlButtons::~ControlButtons()
{
}

//==============================================================================
void ControlButtons::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
}

void ControlButtons::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    int buttonHeight = 30;
    int spacing = 5;
    
    // Clear All button
    auto clearAllBounds = bounds.removeFromTop(buttonHeight);
    clearAllButton.setBounds(clearAllBounds);
    
    bounds.removeFromTop(spacing);
    
    // Pitch Sequencer toggle button
    auto pitchToggleBounds = bounds.removeFromTop(buttonHeight);
    pitchSequencerToggle.setBounds(pitchToggleBounds);
    
    bounds.removeFromTop(spacing);
    
    // Pitch Sequencer length label and dropdown
    auto pitchLengthRow = bounds.removeFromTop(buttonHeight);
    pitchSeqLengthLabel.setBounds(pitchLengthRow.removeFromLeft(80));
    pitchSeqLengthCombo.setBounds(pitchLengthRow);
    
    bounds.removeFromTop(spacing);
    
    // Clear Pitch Sequencer button
    auto clearPitchBounds = bounds.removeFromTop(buttonHeight);
    clearPitchSequencerButton.setBounds(clearPitchBounds);
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
    // Update pitch sequencer toggle button text
    auto& pitchSequencer = patternModel.getPitchSequencer();
    if (pitchSequencer.visible)
    {
        pitchSequencerToggle.setButtonText("Hide Pitch Sequencer");
    }
    else
    {
        pitchSequencerToggle.setButtonText("Show Pitch Sequencer");
    }
    
    // Update pitch sequencer length dropdown based on selected color
    auto& colorConfig = patternModel.getColorConfig(selectedColorChannel);
    int currentLength = colorConfig.pitchSeqLoopLengthBars;
    int itemId = 0;
    
    // Map bar values to combo box item IDs (1-based)
    switch (currentLength)
    {
        case 1: itemId = 1; break;
        case 2: itemId = 2; break;
        case 4: itemId = 3; break;
        case 8: itemId = 4; break;
        case 16: itemId = 5; break;
        case 32: itemId = 6; break;
        default: itemId = 2; break; // Default to 2 bars
    }
    
    pitchSeqLengthCombo.setSelectedId(itemId, juce::dontSendNotification);
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

//==============================================================================
void ControlButtons::setupComponents()
{
    // Clear All button
    clearAllButton.setButtonText("Clear All (Current Color)");
    clearAllButton.onClick = [this]() { onClearAllClicked(); };
    addAndMakeVisible(clearAllButton);
    
    // Pitch Sequencer toggle button
    pitchSequencerToggle.setButtonText("Show Pitch Sequencer");
    pitchSequencerToggle.onClick = [this]() { onPitchSequencerToggleClicked(); };
    addAndMakeVisible(pitchSequencerToggle);
    
    // Pitch Sequencer length label
    pitchSeqLengthLabel.setText("Pitch Len:", juce::dontSendNotification);
    pitchSeqLengthLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(pitchSeqLengthLabel);
    
    // Pitch Sequencer length dropdown
    pitchSeqLengthCombo.addItem("1 Bar", 1);
    pitchSeqLengthCombo.addItem("2 Bars", 2);
    pitchSeqLengthCombo.addItem("4 Bars", 3);
    pitchSeqLengthCombo.addItem("8 Bars", 4);
    pitchSeqLengthCombo.addItem("16 Bars", 5);
    pitchSeqLengthCombo.addItem("32 Bars", 6);
    pitchSeqLengthCombo.setSelectedId(2); // Default to 2 bars
    pitchSeqLengthCombo.onChange = [this]() { onPitchSeqLengthChanged(); };
    addAndMakeVisible(pitchSeqLengthCombo);
    
    // Clear Pitch Sequencer button
    clearPitchSequencerButton.setButtonText("Clear Pitch Sequencer");
    clearPitchSequencerButton.onClick = [this]() { onClearPitchSequencerClicked(); };
    addAndMakeVisible(clearPitchSequencerButton);
}

//==============================================================================
void ControlButtons::onClearAllClicked()
{
    // Clear all squares for the currently selected color channel
    patternModel.clearColorChannel(selectedColorChannel);
}

void ControlButtons::onPitchSequencerToggleClicked()
{
    // Toggle pitch sequencer visibility
    auto& pitchSequencer = patternModel.getPitchSequencer();
    pitchSequencer.visible = !pitchSequencer.visible;
    
    // Update button text
    refreshFromModel();
    
    // Notify listeners
    for (auto* listener : listeners)
    {
        listener->pitchSequencerVisibilityChanged(pitchSequencer.visible);
    }
}

void ControlButtons::onClearPitchSequencerClicked()
{
    // Clear the pitch sequencer waveform for the currently selected color (reset all values to 0)
    auto& colorConfig = patternModel.getColorConfig(selectedColorChannel);
    std::fill(colorConfig.pitchWaveform.begin(), colorConfig.pitchWaveform.end(), 0.0f);
    
    // Trigger a repaint via change message
    patternModel.sendChangeMessage();
}

void ControlButtons::onPitchSeqLengthChanged()
{
    // Map combo box item ID to bar value
    int itemId = pitchSeqLengthCombo.getSelectedId();
    int bars = 2; // Default
    
    switch (itemId)
    {
        case 1: bars = 1; break;
        case 2: bars = 2; break;
        case 3: bars = 4; break;
        case 4: bars = 8; break;
        case 5: bars = 16; break;
        case 6: bars = 32; break;
    }
    
    // Update the pitch sequencer loop length for the selected color
    auto& colorConfig = patternModel.getColorConfig(selectedColorChannel);
    colorConfig.pitchSeqLoopLengthBars = bars;
    
    // Trigger update
    patternModel.sendChangeMessage();
}

} // namespace SquareBeats

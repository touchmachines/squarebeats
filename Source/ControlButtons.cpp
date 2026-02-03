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
    // Update editing mode toggle button text
    auto& pitchSequencer = patternModel.getPitchSequencer();
    if (pitchSequencer.editingPitch)
    {
        pitchSequencerToggle.setButtonText("Edit Squares");
    }
    else
    {
        pitchSequencerToggle.setButtonText("Edit Pitch Sequence");
    }
    
    // Update pitch sequencer length dropdown based on selected color
    auto& colorConfig = patternModel.getColorConfig(selectedColorChannel);
    int currentLength = juce::jlimit(1, 64, colorConfig.pitchSeqLoopLengthBars);
    pitchSeqLengthCombo.setSelectedId(currentLength, juce::dontSendNotification);
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
    
    // Pitch Sequencer toggle button (editing mode toggle)
    pitchSequencerToggle.setButtonText("Edit Pitch Sequence");
    pitchSequencerToggle.onClick = [this]() { onPitchSequencerToggleClicked(); };
    addAndMakeVisible(pitchSequencerToggle);
    
    // Pitch Sequencer length label
    pitchSeqLengthLabel.setText("Pitch Len:", juce::dontSendNotification);
    pitchSeqLengthLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(pitchSeqLengthLabel);
    
    // Pitch Sequencer length dropdown (1-64 bars for polyrhythmic expressions)
    for (int i = 1; i <= 64; ++i)
    {
        juce::String label = (i == 1) ? "1 Bar" : juce::String(i) + " Bars";
        pitchSeqLengthCombo.addItem(label, i);
    }
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
    // Toggle pitch sequencer editing mode
    auto& pitchSequencer = patternModel.getPitchSequencer();
    pitchSequencer.editingPitch = !pitchSequencer.editingPitch;
    
    // Update button text
    refreshFromModel();
    
    // Notify listeners (so PitchSequencerComponent updates mouse interception)
    for (auto* listener : listeners)
    {
        listener->pitchSequencerVisibilityChanged(pitchSequencer.editingPitch);
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
    // Item ID directly corresponds to bar value (1-64)
    int bars = juce::jlimit(1, 64, pitchSeqLengthCombo.getSelectedId());
    
    // Update the pitch sequencer loop length for the selected color
    auto& colorConfig = patternModel.getColorConfig(selectedColorChannel);
    colorConfig.pitchSeqLoopLengthBars = bars;
    
    // Trigger update
    patternModel.sendChangeMessage();
}

} // namespace SquareBeats

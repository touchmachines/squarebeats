#include "ColorConfigPanel.h"

namespace SquareBeats {

//==============================================================================
ColorConfigPanel::ColorConfigPanel(PatternModel& model)
    : patternModel(model)
    , currentColorChannel(0)
{
    setupComponents();
    refreshFromModel();
}

ColorConfigPanel::~ColorConfigPanel()
{
}

//==============================================================================
void ColorConfigPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    
    // Draw border
    g.setColour(juce::Colour(0xff444444));
    g.drawRect(getLocalBounds(), 1);
}

void ColorConfigPanel::resized()
{
    auto bounds = getLocalBounds().reduced(8);
    int rowHeight = 30;
    int labelWidth = 80;
    int spacing = 5;
    int tabHeight = 32;
    
    // Tab buttons at the top
    auto tabArea = bounds.removeFromTop(tabHeight);
    int tabWidth = tabArea.getWidth() / 2;
    notesTabButton.setBounds(tabArea.removeFromLeft(tabWidth));
    pitchTabButton.setBounds(tabArea);
    
    bounds.removeFromTop(spacing);
    
    // Quantization row
    auto quantRow = bounds.removeFromTop(rowHeight);
    quantizationLabel.setBounds(quantRow.removeFromLeft(labelWidth));
    quantRow.removeFromLeft(spacing);
    quantizationCombo.setBounds(quantRow);
    
    bounds.removeFromTop(spacing);
    
    // High note row
    auto highNoteRow = bounds.removeFromTop(rowHeight);
    highNoteLabel.setBounds(highNoteRow.removeFromLeft(labelWidth));
    highNoteRow.removeFromLeft(spacing);
    auto highNoteValueBounds = highNoteRow.removeFromRight(40);
    highNoteRow.removeFromRight(spacing);
    highNoteSlider.setBounds(highNoteRow);
    highNoteValue.setBounds(highNoteValueBounds);
    
    bounds.removeFromTop(spacing);
    
    // Low note row
    auto lowNoteRow = bounds.removeFromTop(rowHeight);
    lowNoteLabel.setBounds(lowNoteRow.removeFromLeft(labelWidth));
    lowNoteRow.removeFromLeft(spacing);
    auto lowNoteValueBounds = lowNoteRow.removeFromRight(40);
    lowNoteRow.removeFromRight(spacing);
    lowNoteSlider.setBounds(lowNoteRow);
    lowNoteValue.setBounds(lowNoteValueBounds);
    
    bounds.removeFromTop(spacing);
    
    // MIDI channel row
    auto midiRow = bounds.removeFromTop(rowHeight);
    midiChannelLabel.setBounds(midiRow.removeFromLeft(labelWidth));
    midiRow.removeFromLeft(spacing);
    midiChannelCombo.setBounds(midiRow);
    
    bounds.removeFromTop(spacing);
    
    // Pitch sequencer length row (always visible)
    auto pitchLenRow = bounds.removeFromTop(rowHeight);
    pitchSeqLengthLabel.setBounds(pitchLenRow.removeFromLeft(labelWidth));
    pitchLenRow.removeFromLeft(spacing);
    pitchSeqLengthCombo.setBounds(pitchLenRow);
    
    bounds.removeFromTop(spacing);
    
    // Clear button at the bottom (context-sensitive)
    auto clearButtonBounds = bounds.removeFromTop(34);
    clearButton.setBounds(clearButtonBounds);
}

//==============================================================================
void ColorConfigPanel::setColorChannel(int colorChannelId)
{
    if (colorChannelId >= 0 && colorChannelId < 4)
    {
        currentColorChannel = colorChannelId;
        refreshFromModel();
    }
}

void ColorConfigPanel::refreshFromModel()
{
    const auto& config = patternModel.getColorConfig(currentColorChannel);
    auto& pitchSeq = patternModel.getPitchSequencer();
    
    // Update tab button states
    notesTabButton.setToggleState(!pitchSeq.editingPitch, juce::dontSendNotification);
    pitchTabButton.setToggleState(pitchSeq.editingPitch, juce::dontSendNotification);
    
    // Update quantization combo
    switch (config.quantize)
    {
        case Q_1_32:  quantizationCombo.setSelectedId(1, juce::dontSendNotification); break;
        case Q_1_16:  quantizationCombo.setSelectedId(2, juce::dontSendNotification); break;
        case Q_1_8:   quantizationCombo.setSelectedId(3, juce::dontSendNotification); break;
        case Q_1_4:   quantizationCombo.setSelectedId(4, juce::dontSendNotification); break;
        case Q_1_2:   quantizationCombo.setSelectedId(5, juce::dontSendNotification); break;
        case Q_1_BAR: quantizationCombo.setSelectedId(6, juce::dontSendNotification); break;
    }
    
    // Update note sliders
    highNoteSlider.setValue(config.highNote, juce::dontSendNotification);
    lowNoteSlider.setValue(config.lowNote, juce::dontSendNotification);
    
    // Update note value labels
    highNoteValue.setText(midiNoteToName(config.highNote), juce::dontSendNotification);
    lowNoteValue.setText(midiNoteToName(config.lowNote), juce::dontSendNotification);
    
    // Update MIDI channel combo
    midiChannelCombo.setSelectedId(config.midiChannel, juce::dontSendNotification);
    
    // Update pitch sequencer length
    pitchSeqLengthCombo.setSelectedId(config.pitchSeqLoopLengthBars, juce::dontSendNotification);
    
    updateControlVisibility();
}

//==============================================================================
void ColorConfigPanel::setupComponents()
{
    // Tab buttons
    notesTabButton.setButtonText("SQUARES");
    notesTabButton.setClickingTogglesState(true);
    notesTabButton.setRadioGroupId(1001);
    notesTabButton.onClick = [this]() { switchToNotesTab(); };
    notesTabButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(notesTabButton);
    
    pitchTabButton.setButtonText("PITCH");
    pitchTabButton.setClickingTogglesState(true);
    pitchTabButton.setRadioGroupId(1001);
    pitchTabButton.onClick = [this]() { switchToPitchTab(); };
    addAndMakeVisible(pitchTabButton);
    
    // Quantization label and combo
    quantizationLabel.setText("Quantization:", juce::dontSendNotification);
    quantizationLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    quantizationLabel.setFont(juce::Font(13.0f));
    addAndMakeVisible(quantizationLabel);
    
    quantizationCombo.addItem("1/32", 1);
    quantizationCombo.addItem("1/16", 2);
    quantizationCombo.addItem("1/8", 3);
    quantizationCombo.addItem("1/4", 4);
    quantizationCombo.addItem("1/2", 5);
    quantizationCombo.addItem("1 Bar", 6);
    quantizationCombo.setSelectedId(2); // Default to 1/16
    quantizationCombo.onChange = [this]() { onQuantizationChanged(); };
    addAndMakeVisible(quantizationCombo);
    
    // High note label, slider, and value
    highNoteLabel.setText("High Note:", juce::dontSendNotification);
    highNoteLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    highNoteLabel.setFont(juce::Font(13.0f));
    addAndMakeVisible(highNoteLabel);
    
    highNoteSlider.setRange(0, 127, 1);
    highNoteSlider.setValue(84); // Default C6
    highNoteSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    highNoteSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    highNoteSlider.onValueChange = [this]() { onHighNoteChanged(); };
    addAndMakeVisible(highNoteSlider);
    
    highNoteValue.setText("C6", juce::dontSendNotification);
    highNoteValue.setColour(juce::Label::textColourId, juce::Colours::white);
    highNoteValue.setJustificationType(juce::Justification::centred);
    highNoteValue.setFont(juce::Font(13.0f));
    addAndMakeVisible(highNoteValue);
    
    // Low note label, slider, and value
    lowNoteLabel.setText("Low Note:", juce::dontSendNotification);
    lowNoteLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    lowNoteLabel.setFont(juce::Font(13.0f));
    addAndMakeVisible(lowNoteLabel);
    
    lowNoteSlider.setRange(0, 127, 1);
    lowNoteSlider.setValue(48); // Default C3
    lowNoteSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    lowNoteSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lowNoteSlider.onValueChange = [this]() { onLowNoteChanged(); };
    addAndMakeVisible(lowNoteSlider);
    
    lowNoteValue.setText("C3", juce::dontSendNotification);
    lowNoteValue.setColour(juce::Label::textColourId, juce::Colours::white);
    lowNoteValue.setJustificationType(juce::Justification::centred);
    lowNoteValue.setFont(juce::Font(13.0f));
    addAndMakeVisible(lowNoteValue);
    
    // MIDI channel label and combo
    midiChannelLabel.setText("MIDI Channel:", juce::dontSendNotification);
    midiChannelLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    midiChannelLabel.setFont(juce::Font(13.0f));
    addAndMakeVisible(midiChannelLabel);
    
    for (int i = 1; i <= 16; ++i)
    {
        midiChannelCombo.addItem(juce::String(i), i);
    }
    midiChannelCombo.setSelectedId(1); // Default to channel 1
    midiChannelCombo.onChange = [this]() { onMidiChannelChanged(); };
    addAndMakeVisible(midiChannelCombo);
    
    // Pitch sequencer length
    pitchSeqLengthLabel.setText("Pitch Len:", juce::dontSendNotification);
    pitchSeqLengthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchSeqLengthLabel.setFont(juce::Font(13.0f));
    addAndMakeVisible(pitchSeqLengthLabel);
    
    for (int i = 1; i <= 64; ++i)
    {
        juce::String label = juce::String(i) + (i == 1 ? " Bar" : " Bars");
        pitchSeqLengthCombo.addItem(label, i);
    }
    pitchSeqLengthCombo.setSelectedId(16); // Default to 16 bars
    pitchSeqLengthCombo.onChange = [this]() { onPitchSeqLengthChanged(); };
    addAndMakeVisible(pitchSeqLengthCombo);
    
    // Context-sensitive clear button
    clearButton.setButtonText("Clear");
    clearButton.onClick = [this]() { onClearClicked(); };
    addAndMakeVisible(clearButton);
}

//==============================================================================
void ColorConfigPanel::updateControlVisibility()
{
    auto& pitchSeq = patternModel.getPitchSequencer();
    bool isPitchMode = pitchSeq.editingPitch;
    
    // Update clear button text based on mode
    if (isPitchMode)
    {
        clearButton.setButtonText("Clear Pitch Sequencer");
    }
    else
    {
        clearButton.setButtonText("Clear All (Current Color)");
    }
    
    // Note controls are always visible but labels change meaning in pitch mode
    if (isPitchMode)
    {
        highNoteLabel.setText("High Pitch:", juce::dontSendNotification);
        lowNoteLabel.setText("Low Pitch:", juce::dontSendNotification);
    }
    else
    {
        highNoteLabel.setText("High Note:", juce::dontSendNotification);
        lowNoteLabel.setText("Low Note:", juce::dontSendNotification);
    }
}

void ColorConfigPanel::switchToNotesTab()
{
    auto& pitchSeq = patternModel.getPitchSequencer();
    pitchSeq.editingPitch = false;
    updateControlVisibility();
    
    // Notify listener about mode change
    if (onEditingModeChanged)
        onEditingModeChanged(false);
    
    patternModel.sendChangeMessage();
}

void ColorConfigPanel::switchToPitchTab()
{
    auto& pitchSeq = patternModel.getPitchSequencer();
    pitchSeq.editingPitch = true;
    updateControlVisibility();
    
    // Notify listener about mode change
    if (onEditingModeChanged)
        onEditingModeChanged(true);
    
    patternModel.sendChangeMessage();
}

void ColorConfigPanel::onClearClicked()
{
    auto& pitchSeq = patternModel.getPitchSequencer();
    
    if (pitchSeq.editingPitch)
    {
        // Clear pitch sequencer for current color
        auto& config = patternModel.getColorConfig(currentColorChannel);
        config.pitchWaveform.clear();
        patternModel.setColorConfig(currentColorChannel, config);
    }
    else
    {
        // Clear all squares of current color
        patternModel.clearColorChannel(currentColorChannel);
    }
}

//==============================================================================
void ColorConfigPanel::onQuantizationChanged()
{
    auto& config = patternModel.getColorConfig(currentColorChannel);
    
    int selectedId = quantizationCombo.getSelectedId();
    switch (selectedId)
    {
        case 1: config.quantize = Q_1_32; break;
        case 2: config.quantize = Q_1_16; break;
        case 3: config.quantize = Q_1_8; break;
        case 4: config.quantize = Q_1_4; break;
        case 5: config.quantize = Q_1_2; break;
        case 6: config.quantize = Q_1_BAR; break;
    }
    
    patternModel.setColorConfig(currentColorChannel, config);
}

void ColorConfigPanel::onHighNoteChanged()
{
    auto& config = patternModel.getColorConfig(currentColorChannel);
    config.highNote = static_cast<int>(highNoteSlider.getValue());
    
    // Update the value label
    highNoteValue.setText(midiNoteToName(config.highNote), juce::dontSendNotification);
    
    patternModel.setColorConfig(currentColorChannel, config);
}

void ColorConfigPanel::onLowNoteChanged()
{
    auto& config = patternModel.getColorConfig(currentColorChannel);
    config.lowNote = static_cast<int>(lowNoteSlider.getValue());
    
    // Update the value label
    lowNoteValue.setText(midiNoteToName(config.lowNote), juce::dontSendNotification);
    
    patternModel.setColorConfig(currentColorChannel, config);
}

void ColorConfigPanel::onMidiChannelChanged()
{
    auto& config = patternModel.getColorConfig(currentColorChannel);
    config.midiChannel = midiChannelCombo.getSelectedId();
    
    patternModel.setColorConfig(currentColorChannel, config);
}

void ColorConfigPanel::onPitchSeqLengthChanged()
{
    auto& config = patternModel.getColorConfig(currentColorChannel);
    config.pitchSeqLoopLengthBars = pitchSeqLengthCombo.getSelectedId();
    
    patternModel.setColorConfig(currentColorChannel, config);
}

//==============================================================================
juce::String ColorConfigPanel::midiNoteToName(int midiNote)
{
    static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    
    int octave = (midiNote / 12) - 1;
    int noteIndex = midiNote % 12;
    
    return juce::String(noteNames[noteIndex]) + juce::String(octave);
}

} // namespace SquareBeats

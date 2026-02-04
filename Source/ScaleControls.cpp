#include "ScaleControls.h"

namespace SquareBeats {

ScaleControls::ScaleControls(PatternModel& model)
    : patternModel(model)
{
    // Root note label
    rootLabel.setText("Root:", juce::dontSendNotification);
    rootLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(rootLabel);
    
    // Root note dropdown
    for (int i = 0; i < 12; ++i) {
        rootNoteCombo.addItem(ScaleConfig::getRootNoteName(static_cast<RootNote>(i)), i + 1);
    }
    rootNoteCombo.setSelectedId(static_cast<int>(patternModel.getScaleConfig().rootNote) + 1, juce::dontSendNotification);
    rootNoteCombo.addListener(this);
    addAndMakeVisible(rootNoteCombo);
    
    // Scale type label
    scaleLabel.setText("Scale:", juce::dontSendNotification);
    scaleLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(scaleLabel);
    
    // Scale type dropdown
    for (int i = 0; i < NUM_SCALE_TYPES; ++i) {
        scaleTypeCombo.addItem(ScaleConfig::getScaleTypeName(static_cast<ScaleType>(i)), i + 1);
    }
    scaleTypeCombo.setSelectedId(static_cast<int>(patternModel.getScaleConfig().scaleType) + 1, juce::dontSendNotification);
    scaleTypeCombo.addListener(this);
    addAndMakeVisible(scaleTypeCombo);
    
    // Active scale label (hidden by default)
    activeScaleLabel.setJustificationType(juce::Justification::centredLeft);
    activeScaleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00ccff));
    addChildComponent(activeScaleLabel);
}

ScaleControls::~ScaleControls()
{
    rootNoteCombo.removeListener(this);
    scaleTypeCombo.removeListener(this);
}

void ScaleControls::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void ScaleControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    
    // Layout: [Root: ][combo] [Scale: ][combo] [Active: label]
    int labelWidth = 45;
    int comboWidth = 55;
    int scaleComboWidth = 110;
    int spacing = 5;
    
    rootLabel.setBounds(bounds.removeFromLeft(labelWidth));
    bounds.removeFromLeft(spacing);
    rootNoteCombo.setBounds(bounds.removeFromLeft(comboWidth));
    bounds.removeFromLeft(spacing * 2);
    
    scaleLabel.setBounds(bounds.removeFromLeft(labelWidth));
    bounds.removeFromLeft(spacing);
    scaleTypeCombo.setBounds(bounds.removeFromLeft(scaleComboWidth));
    
    // Active scale label takes remaining space
    bounds.removeFromLeft(spacing * 2);
    activeScaleLabel.setBounds(bounds);
}

void ScaleControls::refreshFromModel()
{
    const ScaleConfig& config = patternModel.getScaleConfig();
    rootNoteCombo.setSelectedId(static_cast<int>(config.rootNote) + 1, juce::dontSendNotification);
    scaleTypeCombo.setSelectedId(static_cast<int>(config.scaleType) + 1, juce::dontSendNotification);
}

void ScaleControls::setActiveScale(const ScaleConfig* activeScale)
{
    if (activeScale != nullptr) {
        showingActiveScale = true;
        currentActiveScale = *activeScale;
        
        juce::String text = "Playing: ";
        text += ScaleConfig::getRootNoteName(activeScale->rootNote);
        text += " ";
        text += ScaleConfig::getScaleTypeName(activeScale->scaleType);
        
        activeScaleLabel.setText(text, juce::dontSendNotification);
        activeScaleLabel.setVisible(true);
    } else {
        showingActiveScale = false;
        activeScaleLabel.setVisible(false);
    }
}

void ScaleControls::comboBoxChanged(juce::ComboBox* comboBox)
{
    ScaleConfig config = patternModel.getScaleConfig();
    
    if (comboBox == &rootNoteCombo) {
        config.rootNote = static_cast<RootNote>(rootNoteCombo.getSelectedId() - 1);
    }
    else if (comboBox == &scaleTypeCombo) {
        config.scaleType = static_cast<ScaleType>(scaleTypeCombo.getSelectedId() - 1);
    }
    
    patternModel.setScaleConfig(config);
}

} // namespace SquareBeats

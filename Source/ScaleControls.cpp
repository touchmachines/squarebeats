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
    
    // Layout: [Root: ][combo]  [Scale: ][combo]
    int labelWidth = 40;
    int comboWidth = 60;
    int scaleComboWidth = 115;
    int spacing = 5;
    
    rootLabel.setBounds(bounds.removeFromLeft(labelWidth));
    bounds.removeFromLeft(spacing);
    rootNoteCombo.setBounds(bounds.removeFromLeft(comboWidth));
    bounds.removeFromLeft(spacing);
    
    scaleLabel.setBounds(bounds.removeFromLeft(labelWidth));
    bounds.removeFromLeft(spacing);
    scaleTypeCombo.setBounds(bounds.removeFromLeft(scaleComboWidth));
    
    // Active scale label takes remaining space (if needed)
    if (bounds.getWidth() > 0) {
        bounds.removeFromLeft(spacing);
        activeScaleLabel.setBounds(bounds);
    }
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
        
        // Update the dropdowns to show the active scale (but keep them disabled)
        rootNoteCombo.setSelectedId(static_cast<int>(activeScale->rootNote) + 1, juce::dontSendNotification);
        scaleTypeCombo.setSelectedId(static_cast<int>(activeScale->scaleType) + 1, juce::dontSendNotification);
        
        activeScaleLabel.setVisible(false);  // No longer need separate label
    } else {
        showingActiveScale = false;
        activeScaleLabel.setVisible(false);
        
        // Restore the static scale config to the dropdowns
        const ScaleConfig& config = patternModel.getScaleConfig();
        rootNoteCombo.setSelectedId(static_cast<int>(config.rootNote) + 1, juce::dontSendNotification);
        scaleTypeCombo.setSelectedId(static_cast<int>(config.scaleType) + 1, juce::dontSendNotification);
    }
}

void ScaleControls::setControlsEnabled(bool enabled)
{
    controlsEnabled = enabled;
    rootNoteCombo.setEnabled(enabled);
    scaleTypeCombo.setEnabled(enabled);
    
    // Visual feedback - dim the labels when disabled
    auto labelColor = enabled ? juce::Colours::white : juce::Colours::grey;
    rootLabel.setColour(juce::Label::textColourId, labelColor);
    scaleLabel.setColour(juce::Label::textColourId, labelColor);
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

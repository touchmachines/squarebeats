#include "TimeSignatureControls.h"
#include "AppFont.h"

namespace SquareBeats {

//==============================================================================
TimeSignatureControls::TimeSignatureControls(PatternModel& model)
    : patternModel(model)
{
    setupComponents();
    refreshFromModel();
}

TimeSignatureControls::~TimeSignatureControls()
{
}

//==============================================================================
void TimeSignatureControls::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    
    // Draw border
    g.setColour(juce::Colour(0xff444444));
    g.drawRect(getLocalBounds(), 1);
}

void TimeSignatureControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    
    // Title label at the top
    auto titleRow = bounds.removeFromTop(20);
    titleLabel.setBounds(titleRow);
    
    bounds.removeFromTop(5);
    
    // Time signature controls in a row
    auto controlRow = bounds.removeFromTop(25);
    
    int comboWidth = 60;
    int labelWidth = 15;
    int spacing = 5;
    
    numeratorLabel.setBounds(controlRow.removeFromLeft(labelWidth));
    controlRow.removeFromLeft(spacing);
    numeratorCombo.setBounds(controlRow.removeFromLeft(comboWidth));
    controlRow.removeFromLeft(spacing);
    separatorLabel.setBounds(controlRow.removeFromLeft(labelWidth));
    controlRow.removeFromLeft(spacing);
    denominatorLabel.setBounds(controlRow.removeFromLeft(labelWidth));
    controlRow.removeFromLeft(spacing);
    denominatorCombo.setBounds(controlRow.removeFromLeft(comboWidth));
}

//==============================================================================
void TimeSignatureControls::refreshFromModel()
{
    auto timeSignature = patternModel.getTimeSignature();
    
    // Update numerator combo
    numeratorCombo.setSelectedId(timeSignature.numerator, juce::dontSendNotification);
    
    // Update denominator combo
    // Map denominator values to combo IDs
    int denominatorId = 1;
    switch (timeSignature.denominator)
    {
        case 1:  denominatorId = 1; break;
        case 2:  denominatorId = 2; break;
        case 4:  denominatorId = 3; break;
        case 8:  denominatorId = 4; break;
        case 16: denominatorId = 5; break;
        default: denominatorId = 3; break; // Default to 4
    }
    denominatorCombo.setSelectedId(denominatorId, juce::dontSendNotification);
}

//==============================================================================
void TimeSignatureControls::setupComponents()
{
    // Title label
    titleLabel.setText("Time Signature", juce::dontSendNotification);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(AppFont::smallLabel());
    addAndMakeVisible(titleLabel);
    
    // Numerator label
    numeratorLabel.setText("N:", juce::dontSendNotification);
    numeratorLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(numeratorLabel);
    
    // Numerator combo (1-16)
    for (int i = 1; i <= 16; ++i)
    {
        numeratorCombo.addItem(juce::String(i), i);
    }
    numeratorCombo.setSelectedId(4); // Default to 4
    numeratorCombo.onChange = [this]() { onNumeratorChanged(); };
    addAndMakeVisible(numeratorCombo);
    
    // Separator label
    separatorLabel.setText("/", juce::dontSendNotification);
    separatorLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    separatorLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(separatorLabel);
    
    // Denominator label
    denominatorLabel.setText("D:", juce::dontSendNotification);
    denominatorLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(denominatorLabel);
    
    // Denominator combo (1, 2, 4, 8, 16)
    denominatorCombo.addItem("1", 1);
    denominatorCombo.addItem("2", 2);
    denominatorCombo.addItem("4", 3);
    denominatorCombo.addItem("8", 4);
    denominatorCombo.addItem("16", 5);
    denominatorCombo.setSelectedId(3); // Default to 4
    denominatorCombo.onChange = [this]() { onDenominatorChanged(); };
    addAndMakeVisible(denominatorCombo);
}

//==============================================================================
void TimeSignatureControls::onNumeratorChanged()
{
    int numerator = numeratorCombo.getSelectedId();
    auto timeSignature = patternModel.getTimeSignature();
    
    // Validate range (1-16)
    numerator = juce::jlimit(1, 16, numerator);
    
    timeSignature.numerator = numerator;
    patternModel.setTimeSignature(timeSignature.numerator, timeSignature.denominator);
}

void TimeSignatureControls::onDenominatorChanged()
{
    int denominatorId = denominatorCombo.getSelectedId();
    auto timeSignature = patternModel.getTimeSignature();
    
    // Map combo ID to denominator value
    int denominator = 4; // Default
    switch (denominatorId)
    {
        case 1: denominator = 1; break;
        case 2: denominator = 2; break;
        case 3: denominator = 4; break;
        case 4: denominator = 8; break;
        case 5: denominator = 16; break;
    }
    
    timeSignature.denominator = denominator;
    patternModel.setTimeSignature(timeSignature.numerator, timeSignature.denominator);
}

} // namespace SquareBeats

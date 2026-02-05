#include "HelpAboutDialog.h"
#include "AppFont.h"

namespace SquareBeats
{

HelpAboutDialog::HelpAboutDialog()
    : websiteLink("Visit TouchMachines", juce::URL("https://www.touchmachines.com"))
{
    // Configure close button
    closeButton.setButtonText("Close");
    closeButton.onClick = [this]() {
        if (auto* parent = getParentComponent())
        {
            parent->exitModalState(0);
        }
    };
    addAndMakeVisible(closeButton);
    
    // Configure website link
    websiteLink.setFont(AppFont::getFont(16.0f), false);
    addAndMakeVisible(websiteLink);
    
    setSize(600, 750);
}

void HelpAboutDialog::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colour(0xff2a2a2a));
    
    // Border
    g.setColour(juce::Colour(0xff4a4a4a));
    g.drawRect(getLocalBounds(), 2);
    
    auto bounds = getLocalBounds().reduced(30, 20);
    int y = bounds.getY();
    
    // Title
    g.setColour(juce::Colours::white);
    g.setFont(AppFont::getFont(32.0f, true));
    g.drawText("SquareBeats", bounds.getX(), y, bounds.getWidth(), 40, juce::Justification::centred);
    y += 45;
    
    // Version
    g.setFont(AppFont::getFont(16.0f));
    g.setColour(juce::Colour(0xffaaaaaa));
    g.drawText("Version 1.0.0", bounds.getX(), y, bounds.getWidth(), 20, juce::Justification::centred);
    y += 30;
    
    // Description
    g.setColour(juce::Colours::white);
    g.setFont(AppFont::getFont(14.0f));
    juce::String description = "A VST3 MIDI sequencer plugin with a unique square-drawing interface.";
    g.drawFittedText(description, bounds.getX(), y, bounds.getWidth(), 40, juce::Justification::centred, 2);
    y += 50;
    
    // Quick Start section
    g.setFont(AppFont::getFont(18.0f, true));
    g.drawText("Quick Start", bounds.getX(), y, bounds.getWidth(), 25, juce::Justification::left);
    y += 28;
    
    g.setFont(AppFont::getFont(12.0f));
    g.setColour(juce::Colour(0xffdddddd));
    
    juce::StringArray quickStartSteps = {
        "1. Draw squares on the sequencing plane by clicking and dragging",
        "2. Double-click any square to delete it",
        "3. Select different colors for different MIDI channels",
        "4. Use the SQUARES/PITCH tabs to switch editing modes",
        "5. Configure quantization, pitch range, and MIDI channels per color",
        "6. Choose a musical scale to constrain notes",
        "7. Enable Scale Sequencer to chain multiple key changes",
        "8. Select play modes: Forward, Backward, Pendulum, or Probability",
        "9. Press play in your DAW to hear your pattern"
    };
    
    for (const auto& step : quickStartSteps)
    {
        g.drawText(step, bounds.getX(), y, bounds.getWidth(), 18, juce::Justification::left);
        y += 20;
    }
    
    y += 15;
    
    // Features section
    g.setFont(AppFont::getFont(18.0f, true));
    g.setColour(juce::Colours::white);
    g.drawText("Key Features", bounds.getX(), y, bounds.getWidth(), 25, juce::Justification::left);
    y += 28;
    
    g.setFont(AppFont::getFont(12.0f));
    g.setColour(juce::Colour(0xffdddddd));
    
    juce::StringArray features = {
        "- 4 independent color channels with MIDI routing",
        "- Per-color pitch sequencer with polyrhythmic loop lengths",
        "- Scale sequencer for evolving harmonic progressions",
        "- Multiple play modes with probability-based randomization",
        "- Visual feedback with gate flashes and velocity ripples",
        "- Flexible quantization from 1/32 note to 1 bar"
    };
    
    for (const auto& feature : features)
    {
        g.drawText(feature, bounds.getX(), y, bounds.getWidth(), 18, juce::Justification::left);
        y += 20;
    }
    
    y += 25;
    
    // Copyright
    g.setFont(AppFont::getFont(11.0f));
    g.setColour(juce::Colour(0xff888888));
    g.drawText("(c) 2026 TouchMachines. All rights reserved.", 
               bounds.getX(), y, bounds.getWidth(), 20, juce::Justification::centred);
}

void HelpAboutDialog::resized()
{
    auto bounds = getLocalBounds();
    
    // Close button at bottom
    closeButton.setBounds(bounds.getWidth() / 2 - 50, bounds.getHeight() - 50, 100, 30);
    
    // Website link above close button
    websiteLink.setBounds(bounds.getWidth() / 2 - 100, bounds.getHeight() - 90, 200, 30);
}

void HelpAboutDialog::show(juce::Component* parent)
{
    if (parent == nullptr)
        return;
    
    auto dialog = std::make_unique<HelpAboutDialog>();
    
    // Create a dialog window
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(dialog.release());
    options.dialogTitle = "About SquareBeats";
    options.dialogBackgroundColour = juce::Colour(0xff2a2a2a);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = false;
    options.resizable = false;
    options.useBottomRightCornerResizer = false;
    
    options.launchAsync();
}

} // namespace SquareBeats

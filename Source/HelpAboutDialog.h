#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace SquareBeats
{

/**
 * Help/About Dialog
 * 
 * Displays information about SquareBeats including:
 * - Plugin description
 * - Version information
 * - Website link
 * - Copyright information
 * - Quick start guide
 */
class HelpAboutDialog : public juce::Component
{
public:
    HelpAboutDialog();
    ~HelpAboutDialog() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Show the dialog as a modal window
    static void show(juce::Component* parent);

private:
    juce::TextButton closeButton;
    juce::HyperlinkButton websiteLink;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelpAboutDialog)
};

} // namespace SquareBeats

/**
 * Basic compilation test for UI components
 * This test verifies that all UI components can be instantiated
 */

#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#define JUCE_STANDALONE_APPLICATION 1

#include "JuceHeader.h"
#include "PatternModel.h"
#include "ColorSelectorComponent.h"
#include "ColorConfigPanel.h"
#include "LoopLengthSelector.h"
#include "TimeSignatureControls.h"
#include "ControlButtons.h"
#include "PitchSequencerComponent.h"

#include <iostream>
#include <cassert>

using namespace SquareBeats;

void testColorSelectorComponent()
{
    std::cout << "Testing ColorSelectorComponent..." << std::endl;
    
    PatternModel model;
    ColorSelectorComponent selector(model);
    
    // Test initial state
    assert(selector.getSelectedColorChannel() == 0);
    
    // Test setting color channel
    selector.setSelectedColorChannel(2);
    assert(selector.getSelectedColorChannel() == 2);
    
    // Test bounds checking
    selector.setSelectedColorChannel(-1);
    assert(selector.getSelectedColorChannel() == 2); // Should not change
    
    selector.setSelectedColorChannel(5);
    assert(selector.getSelectedColorChannel() == 2); // Should not change
    
    std::cout << "  ✓ ColorSelectorComponent tests passed" << std::endl;
}

void testColorConfigPanel()
{
    std::cout << "Testing ColorConfigPanel..." << std::endl;
    
    PatternModel model;
    ColorConfigPanel panel(model);
    
    // Test initial state
    assert(panel.getColorChannel() == 0);
    
    // Test setting color channel
    panel.setColorChannel(1);
    assert(panel.getColorChannel() == 1);
    
    // Test bounds checking
    panel.setColorChannel(-1);
    assert(panel.getColorChannel() == 1); // Should not change
    
    panel.setColorChannel(4);
    assert(panel.getColorChannel() == 1); // Should not change
    
    std::cout << "  ✓ ColorConfigPanel tests passed" << std::endl;
}

void testLoopLengthSelector()
{
    std::cout << "Testing LoopLengthSelector..." << std::endl;
    
    PatternModel model;
    LoopLengthSelector selector(model);
    
    // Test that it can be created
    // (No public API to test beyond construction)
    
    std::cout << "  ✓ LoopLengthSelector tests passed" << std::endl;
}

void testTimeSignatureControls()
{
    std::cout << "Testing TimeSignatureControls..." << std::endl;
    
    PatternModel model;
    TimeSignatureControls controls(model);
    
    // Test that it can be created
    // (No public API to test beyond construction)
    
    std::cout << "  ✓ TimeSignatureControls tests passed" << std::endl;
}

void testControlButtons()
{
    std::cout << "Testing ControlButtons..." << std::endl;
    
    PatternModel model;
    ControlButtons buttons(model);
    
    // Test initial state
    assert(buttons.getSelectedColorChannel() == 0);
    
    // Test setting color channel
    buttons.setSelectedColorChannel(3);
    assert(buttons.getSelectedColorChannel() == 3);
    
    // Test bounds checking
    buttons.setSelectedColorChannel(-1);
    assert(buttons.getSelectedColorChannel() == 3); // Should not change
    
    buttons.setSelectedColorChannel(4);
    assert(buttons.getSelectedColorChannel() == 3); // Should not change
    
    std::cout << "  ✓ ControlButtons tests passed" << std::endl;
}

void testPitchSequencerComponent()
{
    std::cout << "Testing PitchSequencerComponent..." << std::endl;
    
    PatternModel model;
    PitchSequencerComponent pitchSeq(model);
    
    // Test initial state - should not be in editing mode by default
    auto& pitchSequencer = model.getPitchSequencer();
    assert(!pitchSequencer.editingPitch);
    
    // Component is always visible now (waveform always shows)
    assert(pitchSeq.isVisible());
    
    // Test waveform resolution
    assert(pitchSeq.getWaveformResolution() == 256); // Default resolution
    
    pitchSeq.setWaveformResolution(512);
    assert(pitchSeq.getWaveformResolution() == 512);
    
    // Test editing mode toggle
    pitchSequencer.editingPitch = true;
    pitchSeq.updateVisibility();
    assert(pitchSeq.isVisible()); // Still visible
    
    pitchSequencer.editingPitch = false;
    pitchSeq.updateVisibility();
    assert(pitchSeq.isVisible()); // Still visible (waveform always shows)
    
    std::cout << "  ✓ PitchSequencerComponent tests passed" << std::endl;
}

int main()
{
    std::cout << "Running UI Components Tests..." << std::endl;
    std::cout << "==============================" << std::endl;
    
    try
    {
        testColorSelectorComponent();
        testColorConfigPanel();
        testLoopLengthSelector();
        testTimeSignatureControls();
        testControlButtons();
        testPitchSequencerComponent();
        
        std::cout << std::endl;
        std::cout << "All tests passed!" << std::endl;
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

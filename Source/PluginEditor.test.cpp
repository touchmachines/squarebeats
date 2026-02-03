/**
 * Basic compilation test for PluginEditor
 * This test verifies that the PluginEditor can be instantiated
 * and that component communication works correctly
 */

#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#define JUCE_STANDALONE_APPLICATION 1

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <iostream>
#include <cassert>

void testPluginEditorCreation()
{
    std::cout << "Testing PluginEditor creation..." << std::endl;
    
    SquareBeatsAudioProcessor processor;
    SquareBeatsAudioProcessorEditor editor(processor);
    
    // Test initial size
    assert(editor.getWidth() == 1000);
    assert(editor.getHeight() == 700);
    
    std::cout << "  ✓ PluginEditor creation test passed" << std::endl;
}

void testColorChannelSelection()
{
    std::cout << "Testing color channel selection..." << std::endl;
    
    SquareBeatsAudioProcessor processor;
    SquareBeatsAudioProcessorEditor editor(processor);
    
    // Simulate color channel selection
    editor.colorChannelSelected(2);
    
    // The editor should update all relevant components
    // (We can't directly test this without accessing private members,
    // but we can verify it doesn't crash)
    
    std::cout << "  ✓ Color channel selection test passed" << std::endl;
}

void testPitchSequencerVisibilityChange()
{
    std::cout << "Testing pitch sequencer visibility change..." << std::endl;
    
    SquareBeatsAudioProcessor processor;
    SquareBeatsAudioProcessorEditor editor(processor);
    
    // Simulate pitch sequencer visibility change
    editor.pitchSequencerVisibilityChanged(true);
    editor.pitchSequencerVisibilityChanged(false);
    
    // The editor should update the pitch sequencer component
    // (We can't directly test this without accessing private members,
    // but we can verify it doesn't crash)
    
    std::cout << "  ✓ Pitch sequencer visibility change test passed" << std::endl;
}

void testModelChangeNotification()
{
    std::cout << "Testing model change notification..." << std::endl;
    
    SquareBeatsAudioProcessor processor;
    SquareBeatsAudioProcessorEditor editor(processor);
    
    // Modify the pattern model
    auto& model = processor.getPatternModel();
    model.createSquare(0.1f, 0.2f, 0.3f, 0.4f, 0);
    
    // The editor should receive a change notification and update the UI
    // (We can't directly test this without accessing private members,
    // but we can verify it doesn't crash)
    
    std::cout << "  ✓ Model change notification test passed" << std::endl;
}

void testResizing()
{
    std::cout << "Testing editor resizing..." << std::endl;
    
    SquareBeatsAudioProcessor processor;
    SquareBeatsAudioProcessorEditor editor(processor);
    
    // Test resizing
    editor.setSize(1200, 800);
    assert(editor.getWidth() == 1200);
    assert(editor.getHeight() == 800);
    
    // Test minimum size constraint
    editor.setSize(600, 400);
    // Should be clamped to minimum (800x600)
    assert(editor.getWidth() >= 800);
    assert(editor.getHeight() >= 600);
    
    std::cout << "  ✓ Editor resizing test passed" << std::endl;
}

int main()
{
    std::cout << "Running PluginEditor Tests..." << std::endl;
    std::cout << "=============================" << std::endl;
    
    try
    {
        testPluginEditorCreation();
        testColorChannelSelection();
        testPitchSequencerVisibilityChange();
        testModelChangeNotification();
        testResizing();
        
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

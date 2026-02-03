#include "PluginProcessor.h"
#include <cassert>
#include <iostream>

// Simple test to verify AudioProcessor implementation
int main() {
    std::cout << "Testing SquareBeatsAudioProcessor..." << std::endl;
    
    // Test 1: Constructor and basic initialization
    {
        SquareBeatsAudioProcessor processor;
        assert(processor.getName() == "SquareBeats");
        assert(processor.producesMidi() == true);
        assert(processor.isMidiEffect() == true);
        std::cout << "✓ Constructor and basic properties" << std::endl;
    }
    
    // Test 2: State serialization round-trip
    {
        SquareBeatsAudioProcessor processor;
        
        // Add some data to the pattern model
        auto& model = processor.getPatternModel();
        model.createSquare(0.1f, 0.2f, 0.3f, 0.4f, 0);
        model.setLoopLength(4);
        model.setTimeSignature(3, 4);
        
        // Serialize
        juce::MemoryBlock stateData;
        processor.getStateInformation(stateData);
        
        // Create new processor and deserialize
        SquareBeatsAudioProcessor processor2;
        processor2.setStateInformation(stateData.getData(), static_cast<int>(stateData.getSize()));
        
        // Verify state was restored
        auto& model2 = processor2.getPatternModel();
        assert(model2.getLoopLength() == 4);
        assert(model2.getTimeSignature().numerator == 3);
        assert(model2.getTimeSignature().denominator == 4);
        assert(model2.getAllSquares().size() == 1);
        
        std::cout << "✓ State serialization round-trip" << std::endl;
    }
    
    // Test 3: ProcessBlock doesn't crash with no transport
    {
        SquareBeatsAudioProcessor processor;
        processor.prepareToPlay(44100.0, 512);
        
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midiMessages;
        
        // Should not crash even without transport info
        processor.processBlock(buffer, midiMessages);
        
        processor.releaseResources();
        std::cout << "✓ ProcessBlock without transport" << std::endl;
    }
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}

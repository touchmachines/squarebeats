#include "SequencingPlaneComponent.h"
#include "PatternModel.h"
#include <cassert>
#include <iostream>

using namespace SquareBeats;

/**
 * Basic unit tests for SequencingPlaneComponent
 * These tests verify coordinate conversion and basic functionality.
 */

// Test-friendly subclass that exposes protected methods
class TestableSequencingPlaneComponent : public SequencingPlaneComponent
{
public:
    TestableSequencingPlaneComponent(PatternModel& model) 
        : SequencingPlaneComponent(model) {}
    
    // Expose protected methods for testing
    using SequencingPlaneComponent::pixelXToNormalized;
    using SequencingPlaneComponent::pixelYToNormalized;
};

void testCoordinateConversion()
{
    PatternModel model;
    TestableSequencingPlaneComponent component(model);
    
    // Set a known size for the component
    component.setBounds(0, 0, 800, 600);
    
    // Test pixel to normalized conversion
    float normalizedX = component.pixelXToNormalized(400.0f); // Middle X
    float normalizedY = component.pixelYToNormalized(300.0f); // Middle Y
    
    // Should be approximately 0.5 for both (middle of the component)
    assert(normalizedX >= 0.49f && normalizedX <= 0.51f);
    assert(normalizedY >= 0.49f && normalizedY <= 0.51f);
    
    std::cout << "✓ Coordinate conversion test passed\n";
}

void testPlaybackPosition()
{
    PatternModel model;
    SequencingPlaneComponent component(model);
    
    // Test setting playback position
    component.setPlaybackPosition(0.0f);
    assert(component.getPlaybackPosition() == 0.0f);
    
    component.setPlaybackPosition(0.5f);
    assert(component.getPlaybackPosition() == 0.5f);
    
    component.setPlaybackPosition(1.0f);
    assert(component.getPlaybackPosition() == 1.0f);
    
    std::cout << "✓ Playback position test passed\n";
}

void testColorChannelSelection()
{
    PatternModel model;
    SequencingPlaneComponent component(model);
    
    // Test setting color channel
    component.setSelectedColorChannel(0);
    assert(component.getSelectedColorChannel() == 0);
    
    component.setSelectedColorChannel(2);
    assert(component.getSelectedColorChannel() == 2);
    
    // Test clamping to valid range
    component.setSelectedColorChannel(10);
    assert(component.getSelectedColorChannel() == 3); // Should clamp to max (3)
    
    component.setSelectedColorChannel(-1);
    assert(component.getSelectedColorChannel() == 0); // Should clamp to min (0)
    
    std::cout << "✓ Color channel selection test passed\n";
}

int main()
{
    std::cout << "Running SequencingPlaneComponent tests...\n\n";
    
    try {
        testCoordinateConversion();
        testPlaybackPosition();
        testColorChannelSelection();
        
        std::cout << "\n✓ All SequencingPlaneComponent tests passed!\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}

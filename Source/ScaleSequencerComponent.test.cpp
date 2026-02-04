#include "ScaleSequencerComponent.h"
#include "PatternModel.h"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace SquareBeats;

/**
 * Unit tests for ScaleSequencerComponent note highlighting logic
 * These tests verify the helper functions that determine which keys
 * should be highlighted based on scale intervals and root notes.
 */

// Helper to create a minimal component for testing
class TestableScaleSequencerComponent : public ScaleSequencerComponent
{
public:
    TestableScaleSequencerComponent(PatternModel& model) 
        : ScaleSequencerComponent(model) {}
    
    // Expose protected methods for testing
    using ScaleSequencerComponent::isNoteInScale;
    using ScaleSequencerComponent::isRootNote;
    using ScaleSequencerComponent::getKeyAlpha;
    using ScaleSequencerComponent::shouldRenderKeyboard;
    using ScaleSequencerComponent::calculateKeyboardLayout;
    using ScaleSequencerComponent::getWhiteKeyBounds;
    using ScaleSequencerComponent::getBlackKeyBounds;
};

void testIsNoteInScale_CMajor()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // C Major scale: C, D, E, F, G, A, B (intervals: 0, 2, 4, 5, 7, 9, 11)
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_MAJOR);
    RootNote root = ROOT_C;
    
    // Test notes in scale
    assert(component.isNoteInScale(0, root, intervals));   // C
    assert(component.isNoteInScale(2, root, intervals));   // D
    assert(component.isNoteInScale(4, root, intervals));   // E
    assert(component.isNoteInScale(5, root, intervals));   // F
    assert(component.isNoteInScale(7, root, intervals));   // G
    assert(component.isNoteInScale(9, root, intervals));   // A
    assert(component.isNoteInScale(11, root, intervals));  // B
    
    // Test notes NOT in scale
    assert(!component.isNoteInScale(1, root, intervals));  // C#
    assert(!component.isNoteInScale(3, root, intervals));  // D#
    assert(!component.isNoteInScale(6, root, intervals));  // F#
    assert(!component.isNoteInScale(8, root, intervals));  // G#
    assert(!component.isNoteInScale(10, root, intervals)); // A#
    
    std::cout << "✓ isNoteInScale C Major test passed\n";
}

void testIsNoteInScale_DMajor()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // D Major scale: D, E, F#, G, A, B, C# (intervals: 0, 2, 4, 5, 7, 9, 11 from D)
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_MAJOR);
    RootNote root = ROOT_D;
    
    // Test notes in scale (relative to D)
    assert(component.isNoteInScale(2, root, intervals));   // D (root)
    assert(component.isNoteInScale(4, root, intervals));   // E
    assert(component.isNoteInScale(6, root, intervals));   // F#
    assert(component.isNoteInScale(7, root, intervals));   // G
    assert(component.isNoteInScale(9, root, intervals));   // A
    assert(component.isNoteInScale(11, root, intervals));  // B
    assert(component.isNoteInScale(1, root, intervals));   // C#
    
    // Test notes NOT in scale
    assert(!component.isNoteInScale(0, root, intervals));  // C
    assert(!component.isNoteInScale(3, root, intervals));  // D#
    assert(!component.isNoteInScale(5, root, intervals));  // F
    assert(!component.isNoteInScale(8, root, intervals));  // G#
    assert(!component.isNoteInScale(10, root, intervals)); // A#
    
    std::cout << "✓ isNoteInScale D Major test passed\n";
}

void testIsNoteInScale_AMinor()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // A Natural Minor scale: A, B, C, D, E, F, G (intervals: 0, 2, 3, 5, 7, 8, 10)
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_NATURAL_MINOR);
    RootNote root = ROOT_A;
    
    // Test notes in scale (relative to A)
    assert(component.isNoteInScale(9, root, intervals));   // A (root)
    assert(component.isNoteInScale(11, root, intervals));  // B
    assert(component.isNoteInScale(0, root, intervals));   // C
    assert(component.isNoteInScale(2, root, intervals));   // D
    assert(component.isNoteInScale(4, root, intervals));   // E
    assert(component.isNoteInScale(5, root, intervals));   // F
    assert(component.isNoteInScale(7, root, intervals));   // G
    
    // Test notes NOT in scale
    assert(!component.isNoteInScale(1, root, intervals));  // C#
    assert(!component.isNoteInScale(3, root, intervals));  // D#
    assert(!component.isNoteInScale(6, root, intervals));  // F#
    assert(!component.isNoteInScale(8, root, intervals));  // G#
    assert(!component.isNoteInScale(10, root, intervals)); // A#
    
    std::cout << "✓ isNoteInScale A Minor test passed\n";
}

void testIsNoteInScale_Chromatic()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // Chromatic scale: all 12 notes
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_CHROMATIC);
    RootNote root = ROOT_C;
    
    // All notes should be in scale
    for (int note = 0; note < 12; ++note) {
        assert(component.isNoteInScale(note, root, intervals));
    }
    
    std::cout << "✓ isNoteInScale Chromatic test passed\n";
}

void testIsNoteInScale_Pentatonic()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // C Major Pentatonic: C, D, E, G, A (intervals: 0, 2, 4, 7, 9)
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_PENTATONIC_MAJOR);
    RootNote root = ROOT_C;
    
    // Test notes in scale
    assert(component.isNoteInScale(0, root, intervals));   // C
    assert(component.isNoteInScale(2, root, intervals));   // D
    assert(component.isNoteInScale(4, root, intervals));   // E
    assert(component.isNoteInScale(7, root, intervals));   // G
    assert(component.isNoteInScale(9, root, intervals));   // A
    
    // Test notes NOT in scale (more notes excluded in pentatonic)
    assert(!component.isNoteInScale(1, root, intervals));  // C#
    assert(!component.isNoteInScale(3, root, intervals));  // D#
    assert(!component.isNoteInScale(5, root, intervals));  // F
    assert(!component.isNoteInScale(6, root, intervals));  // F#
    assert(!component.isNoteInScale(8, root, intervals));  // G#
    assert(!component.isNoteInScale(10, root, intervals)); // A#
    assert(!component.isNoteInScale(11, root, intervals)); // B
    
    std::cout << "✓ isNoteInScale Pentatonic test passed\n";
}

void testIsRootNote()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // Test C as root
    assert(component.isRootNote(0, ROOT_C));
    assert(!component.isRootNote(1, ROOT_C));
    assert(!component.isRootNote(11, ROOT_C));
    
    // Test D as root
    assert(component.isRootNote(2, ROOT_D));
    assert(!component.isRootNote(0, ROOT_D));
    assert(!component.isRootNote(3, ROOT_D));
    
    // Test A as root
    assert(component.isRootNote(9, ROOT_A));
    assert(!component.isRootNote(8, ROOT_A));
    assert(!component.isRootNote(10, ROOT_A));
    
    // Test F# as root
    assert(component.isRootNote(6, ROOT_F_SHARP));
    assert(!component.isRootNote(5, ROOT_F_SHARP));
    assert(!component.isRootNote(7, ROOT_F_SHARP));
    
    std::cout << "✓ isRootNote test passed\n";
}

void testGetKeyAlpha_OutOfScale()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // C Major scale
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_MAJOR);
    RootNote root = ROOT_C;
    
    // Test notes NOT in scale should have alpha = 0.1
    float alpha = component.getKeyAlpha(1, root, intervals);  // C#
    assert(std::abs(alpha - 0.1f) < 0.001f);
    
    alpha = component.getKeyAlpha(3, root, intervals);  // D#
    assert(std::abs(alpha - 0.1f) < 0.001f);
    
    alpha = component.getKeyAlpha(6, root, intervals);  // F#
    assert(std::abs(alpha - 0.1f) < 0.001f);
    
    std::cout << "✓ getKeyAlpha out of scale test passed\n";
}

void testGetKeyAlpha_InScale()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // C Major scale
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_MAJOR);
    RootNote root = ROOT_C;
    
    // Test notes in scale (but not root) should have alpha = 0.4
    float alpha = component.getKeyAlpha(2, root, intervals);  // D
    assert(std::abs(alpha - 0.4f) < 0.001f);
    
    alpha = component.getKeyAlpha(4, root, intervals);  // E
    assert(std::abs(alpha - 0.4f) < 0.001f);
    
    alpha = component.getKeyAlpha(7, root, intervals);  // G
    assert(std::abs(alpha - 0.4f) < 0.001f);
    
    std::cout << "✓ getKeyAlpha in scale test passed\n";
}

void testGetKeyAlpha_RootNote()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // C Major scale
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_MAJOR);
    RootNote root = ROOT_C;
    
    // Root note should have alpha = 0.7
    float alpha = component.getKeyAlpha(0, root, intervals);  // C (root)
    assert(std::abs(alpha - 0.7f) < 0.001f);
    
    // Test with different root
    root = ROOT_D;
    alpha = component.getKeyAlpha(2, root, intervals);  // D (root)
    assert(std::abs(alpha - 0.7f) < 0.001f);
    
    // Test with A as root
    root = ROOT_A;
    alpha = component.getKeyAlpha(9, root, intervals);  // A (root)
    assert(std::abs(alpha - 0.7f) < 0.001f);
    
    std::cout << "✓ getKeyAlpha root note test passed\n";
}

void testGetKeyAlpha_AllThreeStates()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // C Major scale: C, D, E, F, G, A, B
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_MAJOR);
    RootNote root = ROOT_C;
    
    // Verify all three alpha states are distinct
    float outOfScaleAlpha = component.getKeyAlpha(1, root, intervals);  // C# (out of scale)
    float inScaleAlpha = component.getKeyAlpha(2, root, intervals);     // D (in scale)
    float rootAlpha = component.getKeyAlpha(0, root, intervals);        // C (root)
    
    // All three should be different
    assert(outOfScaleAlpha < inScaleAlpha);
    assert(inScaleAlpha < rootAlpha);
    
    // Verify exact values
    assert(std::abs(outOfScaleAlpha - 0.1f) < 0.001f);
    assert(std::abs(inScaleAlpha - 0.4f) < 0.001f);
    assert(std::abs(rootAlpha - 0.7f) < 0.001f);
    
    std::cout << "✓ getKeyAlpha all three states test passed\n";
}

void testGetKeyAlpha_ConsistentAcrossScales()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // Test that root note always gets 0.7 alpha regardless of scale type
    RootNote root = ROOT_C;
    
    // Major scale
    auto majorIntervals = ScaleConfig::getScaleIntervals(SCALE_MAJOR);
    float alphaMajor = component.getKeyAlpha(0, root, majorIntervals);
    assert(std::abs(alphaMajor - 0.7f) < 0.001f);
    
    // Minor scale
    auto minorIntervals = ScaleConfig::getScaleIntervals(SCALE_NATURAL_MINOR);
    float alphaMinor = component.getKeyAlpha(0, root, minorIntervals);
    assert(std::abs(alphaMinor - 0.7f) < 0.001f);
    
    // Pentatonic scale
    auto pentatonicIntervals = ScaleConfig::getScaleIntervals(SCALE_PENTATONIC_MAJOR);
    float alphaPentatonic = component.getKeyAlpha(0, root, pentatonicIntervals);
    assert(std::abs(alphaPentatonic - 0.7f) < 0.001f);
    
    // Chromatic scale
    auto chromaticIntervals = ScaleConfig::getScaleIntervals(SCALE_CHROMATIC);
    float alphaChromatic = component.getKeyAlpha(0, root, chromaticIntervals);
    assert(std::abs(alphaChromatic - 0.7f) < 0.001f);
    
    std::cout << "✓ getKeyAlpha consistent across scales test passed\n";
}

// ============================================================================
// Task 6.1: Unit tests for common scale types
// ============================================================================

void testKeyboardRendering_CMajor()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // C Major scale: C, D, E, F, G, A, B
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_MAJOR);
    RootNote root = ROOT_C;
    
    // Verify all 7 notes in C Major have correct highlighting
    // In scale: C(0), D(2), E(4), F(5), G(7), A(9), B(11)
    assert(component.getKeyAlpha(0, root, intervals) == 0.7f);   // C - root
    assert(component.getKeyAlpha(2, root, intervals) == 0.4f);   // D - in scale
    assert(component.getKeyAlpha(4, root, intervals) == 0.4f);   // E - in scale
    assert(component.getKeyAlpha(5, root, intervals) == 0.4f);   // F - in scale
    assert(component.getKeyAlpha(7, root, intervals) == 0.4f);   // G - in scale
    assert(component.getKeyAlpha(9, root, intervals) == 0.4f);   // A - in scale
    assert(component.getKeyAlpha(11, root, intervals) == 0.4f);  // B - in scale
    
    // Out of scale: C#(1), D#(3), F#(6), G#(8), A#(10)
    assert(component.getKeyAlpha(1, root, intervals) == 0.1f);   // C# - out
    assert(component.getKeyAlpha(3, root, intervals) == 0.1f);   // D# - out
    assert(component.getKeyAlpha(6, root, intervals) == 0.1f);   // F# - out
    assert(component.getKeyAlpha(8, root, intervals) == 0.1f);   // G# - out
    assert(component.getKeyAlpha(10, root, intervals) == 0.1f);  // A# - out
    
    std::cout << "✓ Keyboard rendering C Major test passed\n";
}

void testKeyboardRendering_AMinor()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // A Natural Minor scale: A, B, C, D, E, F, G
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_NATURAL_MINOR);
    RootNote root = ROOT_A;
    
    // Verify all 7 notes in A Minor have correct highlighting
    // In scale: A(9), B(11), C(0), D(2), E(4), F(5), G(7)
    assert(component.getKeyAlpha(9, root, intervals) == 0.7f);   // A - root
    assert(component.getKeyAlpha(11, root, intervals) == 0.4f);  // B - in scale
    assert(component.getKeyAlpha(0, root, intervals) == 0.4f);   // C - in scale
    assert(component.getKeyAlpha(2, root, intervals) == 0.4f);   // D - in scale
    assert(component.getKeyAlpha(4, root, intervals) == 0.4f);   // E - in scale
    assert(component.getKeyAlpha(5, root, intervals) == 0.4f);   // F - in scale
    assert(component.getKeyAlpha(7, root, intervals) == 0.4f);   // G - in scale
    
    // Out of scale: C#(1), D#(3), F#(6), G#(8), A#(10)
    assert(component.getKeyAlpha(1, root, intervals) == 0.1f);   // C# - out
    assert(component.getKeyAlpha(3, root, intervals) == 0.1f);   // D# - out
    assert(component.getKeyAlpha(6, root, intervals) == 0.1f);   // F# - out
    assert(component.getKeyAlpha(8, root, intervals) == 0.1f);   // G# - out
    assert(component.getKeyAlpha(10, root, intervals) == 0.1f);  // A# - out
    
    std::cout << "✓ Keyboard rendering A Minor test passed\n";
}

void testKeyboardRendering_Chromatic()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // Chromatic scale: all 12 notes
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_CHROMATIC);
    RootNote root = ROOT_C;
    
    // Root note should have 0.7 alpha
    assert(component.getKeyAlpha(0, root, intervals) == 0.7f);  // C - root
    
    // All other notes should be in scale (0.4 alpha)
    for (int note = 1; note < 12; ++note) {
        assert(component.getKeyAlpha(note, root, intervals) == 0.4f);
    }
    
    std::cout << "✓ Keyboard rendering Chromatic test passed\n";
}

void testKeyboardRendering_Pentatonic()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // C Major Pentatonic: C, D, E, G, A (only 5 notes)
    auto intervals = ScaleConfig::getScaleIntervals(SCALE_PENTATONIC_MAJOR);
    RootNote root = ROOT_C;
    
    // In scale: C(0), D(2), E(4), G(7), A(9)
    assert(component.getKeyAlpha(0, root, intervals) == 0.7f);   // C - root
    assert(component.getKeyAlpha(2, root, intervals) == 0.4f);   // D - in scale
    assert(component.getKeyAlpha(4, root, intervals) == 0.4f);   // E - in scale
    assert(component.getKeyAlpha(7, root, intervals) == 0.4f);   // G - in scale
    assert(component.getKeyAlpha(9, root, intervals) == 0.4f);   // A - in scale
    
    // Out of scale (7 notes): C#(1), D#(3), F(5), F#(6), G#(8), A#(10), B(11)
    assert(component.getKeyAlpha(1, root, intervals) == 0.1f);   // C# - out
    assert(component.getKeyAlpha(3, root, intervals) == 0.1f);   // D# - out
    assert(component.getKeyAlpha(5, root, intervals) == 0.1f);   // F - out
    assert(component.getKeyAlpha(6, root, intervals) == 0.1f);   // F# - out
    assert(component.getKeyAlpha(8, root, intervals) == 0.1f);   // G# - out
    assert(component.getKeyAlpha(10, root, intervals) == 0.1f);  // A# - out
    assert(component.getKeyAlpha(11, root, intervals) == 0.1f);  // B - out
    
    std::cout << "✓ Keyboard rendering Pentatonic test passed\n";
}

// ============================================================================
// Task 6.2: Unit tests for edge cases
// ============================================================================

void testKeyboardVisibility_ExactThreshold()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // Test segment width exactly 60 pixels (threshold boundary)
    juce::Rectangle<float> segmentBounds(0.0f, 0.0f, 60.0f, 50.0f);
    
    // Should render at exactly 60 pixels
    assert(component.shouldRenderKeyboard(segmentBounds));
    
    std::cout << "✓ Keyboard visibility at exact threshold (60px) test passed\n";
}

void testKeyboardVisibility_BelowThreshold()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // Test segment width 59 pixels (just below threshold)
    juce::Rectangle<float> segmentBounds(0.0f, 0.0f, 59.0f, 50.0f);
    
    // Should NOT render below 60 pixels
    assert(!component.shouldRenderKeyboard(segmentBounds));
    
    std::cout << "✓ Keyboard visibility below threshold (59px) test passed\n";
}

void testKeyboardVisibility_VeryWideSegment()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // Test very wide segment (200+ pixels)
    juce::Rectangle<float> segmentBounds(0.0f, 0.0f, 250.0f, 50.0f);
    
    // Should render for wide segments
    assert(component.shouldRenderKeyboard(segmentBounds));
    
    // Calculate layout for wide segment
    auto layout = component.calculateKeyboardLayout(segmentBounds);
    assert(layout.shouldRender);
    
    // Verify keyboard bounds are reasonable
    assert(layout.keyboardBounds.getWidth() > 0.0f);
    assert(layout.keyboardBounds.getHeight() > 0.0f);
    assert(layout.whiteKeyWidth > 0.0f);
    assert(layout.blackKeyWidth > 0.0f);
    
    // Black keys should be smaller than white keys
    assert(layout.blackKeyWidth < layout.whiteKeyWidth);
    assert(layout.blackKeyHeight < layout.whiteKeyHeight);
    
    std::cout << "✓ Keyboard visibility for very wide segment (250px) test passed\n";
}

void testKeyboardLayout_MinimumHeight()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // Test segment with minimum height (20 pixels)
    juce::Rectangle<float> segmentBounds(0.0f, 0.0f, 100.0f, 20.0f);
    
    // Should render at minimum height
    assert(component.shouldRenderKeyboard(segmentBounds));
    
    // Test segment below minimum height (19 pixels)
    juce::Rectangle<float> tooShort(0.0f, 0.0f, 100.0f, 19.0f);
    
    // Should NOT render below minimum height
    assert(!component.shouldRenderKeyboard(tooShort));
    
    std::cout << "✓ Keyboard layout minimum height test passed\n";
}

void testKeyboardLayout_KeyPositions()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // Test with a standard segment size
    juce::Rectangle<float> segmentBounds(10.0f, 10.0f, 140.0f, 50.0f);
    
    auto layout = component.calculateKeyboardLayout(segmentBounds);
    assert(layout.shouldRender);
    
    // Verify white key positions are sequential and non-overlapping
    for (int i = 0; i < 6; ++i) {
        auto key1 = component.getWhiteKeyBounds(layout, i);
        auto key2 = component.getWhiteKeyBounds(layout, i + 1);
        
        // Keys should be adjacent (right edge of key1 should equal left edge of key2)
        assert(std::abs(key1.getRight() - key2.getX()) < 0.01f);
    }
    
    // Verify black key positions are between white keys
    // C# should be between C and D
    auto cKey = component.getWhiteKeyBounds(layout, 0);  // C
    auto dKey = component.getWhiteKeyBounds(layout, 1);  // D
    auto cSharpKey = component.getBlackKeyBounds(layout, 0);  // C#
    
    // C# should be positioned between C and D
    assert(cSharpKey.getCentreX() > cKey.getX());
    assert(cSharpKey.getCentreX() < dKey.getRight());
    
    std::cout << "✓ Keyboard layout key positions test passed\n";
}

void testKeyboardLayout_BlackKeySizing()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // Test with various segment sizes
    juce::Rectangle<float> segmentBounds(0.0f, 0.0f, 140.0f, 50.0f);
    
    auto layout = component.calculateKeyboardLayout(segmentBounds);
    assert(layout.shouldRender);
    
    // Black keys should be approximately 60% of white key size
    float expectedBlackWidth = layout.whiteKeyWidth * 0.6f;
    float expectedBlackHeight = layout.whiteKeyHeight * 0.6f;
    
    assert(std::abs(layout.blackKeyWidth - expectedBlackWidth) < 0.01f);
    assert(std::abs(layout.blackKeyHeight - expectedBlackHeight) < 0.01f);
    
    // Verify all black keys have consistent sizing
    for (int i = 0; i < 5; ++i) {
        auto blackKey = component.getBlackKeyBounds(layout, i);
        assert(std::abs(blackKey.getWidth() - layout.blackKeyWidth) < 0.01f);
        assert(std::abs(blackKey.getHeight() - layout.blackKeyHeight) < 0.01f);
    }
    
    std::cout << "✓ Keyboard layout black key sizing test passed\n";
}

void testKeyboardLayout_NonOverlappingWithText()
{
    PatternModel model;
    TestableScaleSequencerComponent component(model);
    
    // Test with a segment that has text
    juce::Rectangle<float> segmentBounds(0.0f, 0.0f, 140.0f, 50.0f);
    
    auto layout = component.calculateKeyboardLayout(segmentBounds);
    assert(layout.shouldRender);
    
    // Keyboard should be in the bottom portion of the segment
    // Text occupies approximately 60% of segment height
    float textHeight = segmentBounds.getHeight() * 0.6f;
    float expectedKeyboardTop = segmentBounds.getY() + textHeight;
    
    // Keyboard should start below the text area
    assert(layout.keyboardBounds.getY() >= expectedKeyboardTop - 0.01f);
    
    // Keyboard should not extend beyond segment bounds
    assert(layout.keyboardBounds.getBottom() <= segmentBounds.getBottom());
    assert(layout.keyboardBounds.getX() >= segmentBounds.getX());
    assert(layout.keyboardBounds.getRight() <= segmentBounds.getRight());
    
    std::cout << "✓ Keyboard layout non-overlapping with text test passed\n";
}

int main()
{
    std::cout << "Running ScaleSequencerComponent note highlighting tests...\n\n";
    
    try
    {
        // Original tests
        testIsNoteInScale_CMajor();
        testIsNoteInScale_DMajor();
        testIsNoteInScale_AMinor();
        testIsNoteInScale_Chromatic();
        testIsNoteInScale_Pentatonic();
        testIsRootNote();
        testGetKeyAlpha_OutOfScale();
        testGetKeyAlpha_InScale();
        testGetKeyAlpha_RootNote();
        testGetKeyAlpha_AllThreeStates();
        testGetKeyAlpha_ConsistentAcrossScales();
        
        std::cout << "\n--- Task 6.1: Common Scale Types Tests ---\n";
        testKeyboardRendering_CMajor();
        testKeyboardRendering_AMinor();
        testKeyboardRendering_Chromatic();
        testKeyboardRendering_Pentatonic();
        
        std::cout << "\n--- Task 6.2: Edge Cases Tests ---\n";
        testKeyboardVisibility_ExactThreshold();
        testKeyboardVisibility_BelowThreshold();
        testKeyboardVisibility_VeryWideSegment();
        testKeyboardLayout_MinimumHeight();
        testKeyboardLayout_KeyPositions();
        testKeyboardLayout_BlackKeySizing();
        testKeyboardLayout_NonOverlappingWithText();
        
        std::cout << "\n✓ All tests passed!\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}

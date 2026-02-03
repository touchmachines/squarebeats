#include "PatternModel.h"
#include <cassert>
#include <iostream>

using namespace SquareBeats;

/**
 * Basic unit tests for PatternModel
 * These tests verify core functionality of square management,
 * color channel configuration, and global settings.
 */

void testSquareCreation()
{
    PatternModel model;
    
    // Create a square
    Square* square = model.createSquare(0.25f, 0.5f, 0.1f, 0.2f, 0);
    
    assert(square != nullptr);
    assert(square->leftEdge == 0.25f);
    assert(square->topEdge == 0.5f);
    assert(square->width == 0.1f);
    assert(square->height == 0.2f);
    assert(square->colorChannelId == 0);
    assert(square->uniqueId > 0);
    
    std::cout << "✓ Square creation test passed\n";
}

void testSquareMovement()
{
    PatternModel model;
    
    Square* square = model.createSquare(0.25f, 0.5f, 0.1f, 0.2f, 0);
    uint32_t id = square->uniqueId;
    
    // Move the square
    bool success = model.moveSquare(id, 0.5f, 0.3f);
    
    assert(success);
    assert(square->leftEdge == 0.5f);
    assert(square->topEdge == 0.3f);
    assert(square->width == 0.1f);  // Width unchanged
    assert(square->height == 0.2f); // Height unchanged
    assert(square->uniqueId == id); // ID preserved
    
    std::cout << "✓ Square movement test passed\n";
}

void testSquareResize()
{
    PatternModel model;
    
    Square* square = model.createSquare(0.25f, 0.5f, 0.1f, 0.2f, 0);
    uint32_t id = square->uniqueId;
    
    // Resize the square
    bool success = model.resizeSquare(id, 0.15f, 0.25f);
    
    assert(success);
    assert(square->leftEdge == 0.25f); // Position unchanged
    assert(square->topEdge == 0.5f);   // Position unchanged
    assert(square->width == 0.15f);
    assert(square->height == 0.25f);
    assert(square->uniqueId == id);    // ID preserved
    
    std::cout << "✓ Square resize test passed\n";
}

void testSquareDeletion()
{
    PatternModel model;
    
    Square* square = model.createSquare(0.25f, 0.5f, 0.1f, 0.2f, 0);
    uint32_t id = square->uniqueId;
    
    // Verify square exists
    auto squares = model.getAllSquares();
    assert(squares.size() == 1);
    
    // Delete the square
    bool success = model.deleteSquare(id);
    
    assert(success);
    
    // Verify square is gone
    squares = model.getAllSquares();
    assert(squares.empty());
    
    std::cout << "✓ Square deletion test passed\n";
}

void testClearColorChannel()
{
    PatternModel model;
    
    // Create squares with different colors
    model.createSquare(0.1f, 0.1f, 0.1f, 0.1f, 0);
    model.createSquare(0.2f, 0.2f, 0.1f, 0.1f, 1);
    model.createSquare(0.3f, 0.3f, 0.1f, 0.1f, 0);
    model.createSquare(0.4f, 0.4f, 0.1f, 0.1f, 2);
    
    auto squares = model.getAllSquares();
    assert(squares.size() == 4);
    
    // Clear color channel 0
    model.clearColorChannel(0);
    
    squares = model.getAllSquares();
    assert(squares.size() == 2);
    
    // Verify only color 1 and 2 remain
    for (auto* sq : squares)
    {
        assert(sq->colorChannelId != 0);
    }
    
    std::cout << "✓ Clear color channel test passed\n";
}

void testGetSquaresInTimeRange()
{
    PatternModel model;
    
    // Create squares at different time positions
    model.createSquare(0.1f, 0.5f, 0.1f, 0.1f, 0);  // 0.1 to 0.2
    model.createSquare(0.3f, 0.5f, 0.1f, 0.1f, 0);  // 0.3 to 0.4
    model.createSquare(0.5f, 0.5f, 0.1f, 0.1f, 0);  // 0.5 to 0.6
    
    // Query range [0.25, 0.55]
    auto squares = model.getSquaresInTimeRange(0.25f, 0.55f);
    
    // Should get squares at 0.3 and 0.5
    assert(squares.size() == 2);
    
    std::cout << "✓ Get squares in time range test passed\n";
}

void testColorChannelConfiguration()
{
    PatternModel model;
    
    // Get default config
    const auto& config = model.getColorConfig(0);
    assert(config.midiChannel == 1);
    assert(config.highNote == 84);
    assert(config.lowNote == 48);
    assert(config.quantize == Q_1_16);
    
    // Modify config
    ColorChannelConfig newConfig;
    newConfig.midiChannel = 5;
    newConfig.highNote = 96;
    newConfig.lowNote = 36;
    newConfig.quantize = Q_1_8;
    
    model.setColorConfig(1, newConfig);
    
    const auto& retrievedConfig = model.getColorConfig(1);
    assert(retrievedConfig.midiChannel == 5);
    assert(retrievedConfig.highNote == 96);
    assert(retrievedConfig.lowNote == 36);
    assert(retrievedConfig.quantize == Q_1_8);
    
    std::cout << "✓ Color channel configuration test passed\n";
}

void testLoopLength()
{
    PatternModel model;
    
    // Default should be 2 bars
    assert(model.getLoopLength() == 2);
    
    // Set to 4 bars
    model.setLoopLength(4);
    assert(model.getLoopLength() == 4);
    
    // Set to 1 bar
    model.setLoopLength(1);
    assert(model.getLoopLength() == 1);
    
    std::cout << "✓ Loop length test passed\n";
}

void testTimeSignature()
{
    PatternModel model;
    
    // Default should be 4/4
    auto timeSig = model.getTimeSignature();
    assert(timeSig.numerator == 4);
    assert(timeSig.denominator == 4);
    
    // Set to 3/4
    model.setTimeSignature(3, 4);
    timeSig = model.getTimeSignature();
    assert(timeSig.numerator == 3);
    assert(timeSig.denominator == 4);
    
    // Set to 7/8
    model.setTimeSignature(7, 8);
    timeSig = model.getTimeSignature();
    assert(timeSig.numerator == 7);
    assert(timeSig.denominator == 8);
    
    std::cout << "✓ Time signature test passed\n";
}

void testPitchSequencer()
{
    PatternModel model;
    
    auto& pitchSeq = model.getPitchSequencer();
    
    // Default should be invisible with 2 bar loop
    assert(!pitchSeq.visible);
    assert(pitchSeq.loopLengthBars == 2);
    assert(pitchSeq.waveform.empty());
    
    // Modify pitch sequencer
    pitchSeq.visible = true;
    pitchSeq.loopLengthBars = 4;
    pitchSeq.waveform = {0.0f, 1.0f, 2.0f, 1.0f, 0.0f};
    
    const auto& retrievedSeq = model.getPitchSequencer();
    assert(retrievedSeq.visible);
    assert(retrievedSeq.loopLengthBars == 4);
    assert(retrievedSeq.waveform.size() == 5);
    
    std::cout << "✓ Pitch sequencer test passed\n";
}

void testPitchSequencerGetOffsetAt()
{
    PatternModel model;
    auto& pitchSeq = model.getPitchSequencer();
    
    // Test 1: Empty waveform returns 0
    pitchSeq.visible = true;
    assert(pitchSeq.getPitchOffsetAt(0.5) == 0.0f);
    
    // Test 2: Hidden sequencer returns 0
    pitchSeq.waveform = {1.0f, 2.0f, 3.0f};
    pitchSeq.visible = false;
    assert(pitchSeq.getPitchOffsetAt(0.5) == 0.0f);
    
    // Test 3: Linear interpolation
    pitchSeq.visible = true;
    pitchSeq.waveform = {0.0f, 4.0f};  // Simple case: 0 at start, 4 at end
    
    // At position 0.0, should be 0.0
    float offset = pitchSeq.getPitchOffsetAt(0.0);
    assert(std::abs(offset - 0.0f) < 0.01f);
    
    // At position 0.5, should be 2.0 (halfway between 0 and 4)
    offset = pitchSeq.getPitchOffsetAt(0.5);
    assert(std::abs(offset - 2.0f) < 0.01f);
    
    // At position 1.0, should be 4.0
    offset = pitchSeq.getPitchOffsetAt(1.0);
    assert(std::abs(offset - 4.0f) < 0.01f);
    
    // Test 4: Multi-segment interpolation
    pitchSeq.waveform = {0.0f, 2.0f, 1.0f};  // 3 points
    
    // At position 0.25 (halfway between index 0 and 1)
    offset = pitchSeq.getPitchOffsetAt(0.25);
    assert(std::abs(offset - 1.0f) < 0.01f);
    
    // At position 0.75 (halfway between index 1 and 2)
    offset = pitchSeq.getPitchOffsetAt(0.75);
    assert(std::abs(offset - 1.5f) < 0.01f);
    
    // Test 5: Position wrapping (> 1.0)
    offset = pitchSeq.getPitchOffsetAt(1.5);  // Should wrap to 0.5
    float expectedOffset = pitchSeq.getPitchOffsetAt(0.5);
    assert(std::abs(offset - expectedOffset) < 0.01f);
    
    std::cout << "✓ Pitch sequencer getPitchOffsetAt test passed\n";
}

void testLoopLengthChangePreservesSquares()
{
    PatternModel model;
    
    // Start with 4 bar loop
    model.setLoopLength(4);
    
    // Create squares at different positions
    // Normalized coordinates are relative to current loop length
    auto* sq1 = model.createSquare(0.1f, 0.5f, 0.1f, 0.1f, 0);  // Early in loop
    auto* sq2 = model.createSquare(0.4f, 0.5f, 0.1f, 0.1f, 1);  // Middle of loop
    auto* sq3 = model.createSquare(0.7f, 0.5f, 0.1f, 0.1f, 2);  // Late in loop
    
    uint32_t id1 = sq1->uniqueId;
    uint32_t id2 = sq2->uniqueId;
    uint32_t id3 = sq3->uniqueId;
    
    assert(model.getAllSquares().size() == 3);
    
    // Change to 2 bar loop (half the length)
    // Squares at normalized position >= 0.5 should be removed
    model.setLoopLength(2);
    
    auto squares = model.getAllSquares();
    assert(squares.size() == 2);  // sq1 and sq2 should remain, sq3 should be removed
    
    // Verify the correct squares remain
    bool foundSq1 = false;
    bool foundSq2 = false;
    bool foundSq3 = false;
    
    for (auto* sq : squares)
    {
        if (sq->uniqueId == id1) foundSq1 = true;
        if (sq->uniqueId == id2) foundSq2 = true;
        if (sq->uniqueId == id3) foundSq3 = true;
    }
    
    assert(foundSq1);  // Should be preserved (at 0.1)
    assert(foundSq2);  // Should be preserved (at 0.4)
    assert(!foundSq3); // Should be removed (at 0.7 >= 0.5)
    
    std::cout << "✓ Loop length change preserves squares test passed\n";
}

void testLoopLengthIncreasePreservesAllSquares()
{
    PatternModel model;
    
    // Start with 1 bar loop
    model.setLoopLength(1);
    
    // Create squares
    model.createSquare(0.2f, 0.5f, 0.1f, 0.1f, 0);
    model.createSquare(0.5f, 0.5f, 0.1f, 0.1f, 1);
    model.createSquare(0.8f, 0.5f, 0.1f, 0.1f, 2);
    
    assert(model.getAllSquares().size() == 3);
    
    // Increase to 4 bars - all squares should be preserved
    model.setLoopLength(4);
    
    assert(model.getAllSquares().size() == 3);
    
    std::cout << "✓ Loop length increase preserves all squares test passed\n";
}

void testLoopLengthDecreaseBoundaryCase()
{
    PatternModel model;
    
    // Start with 4 bar loop
    model.setLoopLength(4);
    
    // Create squares exactly at boundaries
    auto* sq1 = model.createSquare(0.49f, 0.5f, 0.1f, 0.1f, 0);  // Just before 0.5
    auto* sq2 = model.createSquare(0.50f, 0.5f, 0.1f, 0.1f, 1);  // Exactly at 0.5
    
    uint32_t id1 = sq1->uniqueId;
    uint32_t id2 = sq2->uniqueId;
    
    // Change to 2 bar loop (threshold at 0.5)
    model.setLoopLength(2);
    
    auto squares = model.getAllSquares();
    
    // sq1 (at 0.49) should be preserved
    // sq2 (at 0.50) should be removed (>= threshold)
    assert(squares.size() == 1);
    assert(squares[0]->uniqueId == id1);
    
    std::cout << "✓ Loop length decrease boundary case test passed\n";
}

int main()
{
    std::cout << "Running PatternModel unit tests...\n\n";
    
    try
    {
        testSquareCreation();
        testSquareMovement();
        testSquareResize();
        testSquareDeletion();
        testClearColorChannel();
        testGetSquaresInTimeRange();
        testColorChannelConfiguration();
        testLoopLength();
        testTimeSignature();
        testPitchSequencer();
        testPitchSequencerGetOffsetAt();
        testLoopLengthChangePreservesSquares();
        testLoopLengthIncreasePreservesAllSquares();
        testLoopLengthDecreaseBoundaryCase();
        
        std::cout << "\n✓ All tests passed!\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}

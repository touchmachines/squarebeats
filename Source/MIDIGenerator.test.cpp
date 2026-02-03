#include "MIDIGenerator.h"
#include <cassert>
#include <cmath>
#include <iostream>

using namespace SquareBeats;

//==============================================================================
// Test helpers

void testCalculateMidiNote() {
    std::cout << "Testing calculateMidiNote()..." << std::endl;
    
    ColorChannelConfig config;
    config.highNote = 84;  // C6
    config.lowNote = 48;   // C3
    
    // Test square at top (should map to high note)
    Square topSquare(0.0f, 0.0f, 0.1f, 0.1f, 0, 1);
    int topNote = MIDIGenerator::calculateMidiNote(topSquare, config, 0.0f);
    assert(topNote == 84);
    std::cout << "  Top position: " << topNote << " (expected 84)" << std::endl;
    
    // Test square at bottom (should map to low note)
    Square bottomSquare(0.0f, 0.9f, 0.1f, 0.1f, 0, 2);
    int bottomNote = MIDIGenerator::calculateMidiNote(bottomSquare, config, 0.0f);
    assert(bottomNote == 48);
    std::cout << "  Bottom position: " << bottomNote << " (expected 48)" << std::endl;
    
    // Test square in middle (should map to middle note)
    Square midSquare(0.0f, 0.45f, 0.1f, 0.1f, 0, 3);
    int midNote = MIDIGenerator::calculateMidiNote(midSquare, config, 0.0f);
    // Center Y = 0.45 + 0.05 = 0.5, should map to (84 + 48) / 2 = 66
    assert(midNote == 66);
    std::cout << "  Middle position: " << midNote << " (expected 66)" << std::endl;
    
    // Test with pitch offset
    Square offsetSquare(0.0f, 0.0f, 0.1f, 0.1f, 0, 4);
    int offsetNote = MIDIGenerator::calculateMidiNote(offsetSquare, config, 12.0f);
    assert(offsetNote == 96);  // 84 + 12
    std::cout << "  With +12 offset: " << offsetNote << " (expected 96)" << std::endl;
    
    // Test clamping to valid MIDI range (high)
    int clampedHighNote = MIDIGenerator::calculateMidiNote(offsetSquare, config, 50.0f);
    assert(clampedHighNote == 127);
    std::cout << "  Clamped high: " << clampedHighNote << " (expected 127)" << std::endl;
    
    // Test clamping to valid MIDI range (low)
    Square lowSquare(0.0f, 0.9f, 0.1f, 0.1f, 0, 5);
    int clampedLowNote = MIDIGenerator::calculateMidiNote(lowSquare, config, -50.0f);
    assert(clampedLowNote == 0);
    std::cout << "  Clamped low: " << clampedLowNote << " (expected 0)" << std::endl;
    
    std::cout << "  ✓ All calculateMidiNote tests passed" << std::endl;
}

void testCalculateVelocity() {
    std::cout << "Testing calculateVelocity()..." << std::endl;
    
    // Test minimum height (should give velocity 1)
    Square minSquare(0.0f, 0.0f, 0.1f, 0.0f, 0, 1);
    int minVel = MIDIGenerator::calculateVelocity(minSquare);
    assert(minVel == 1);
    std::cout << "  Min height: " << minVel << " (expected 1)" << std::endl;
    
    // Test maximum height (should give velocity 127)
    Square maxSquare(0.0f, 0.0f, 0.1f, 1.0f, 0, 2);
    int maxVel = MIDIGenerator::calculateVelocity(maxSquare);
    assert(maxVel == 127);
    std::cout << "  Max height: " << maxVel << " (expected 127)" << std::endl;
    
    // Test middle height (should give velocity around 64)
    Square midSquare(0.0f, 0.0f, 0.1f, 0.5f, 0, 3);
    int midVel = MIDIGenerator::calculateVelocity(midSquare);
    assert(midVel >= 63 && midVel <= 65);  // Allow for rounding
    std::cout << "  Mid height: " << midVel << " (expected ~64)" << std::endl;
    
    std::cout << "  ✓ All calculateVelocity tests passed" << std::endl;
}

void testApplyQuantization() {
    std::cout << "Testing applyQuantization()..." << std::endl;
    
    TimeSignature timeSig(4, 4);  // 4/4 time, 4 beats per bar
    
    // Test 1/16 note quantization
    double time1 = 0.3;  // Should quantize to 0.25 (1/16 of 4 beats = 0.25)
    double quantized1 = MIDIGenerator::applyQuantization(time1, Q_1_16, timeSig);
    assert(std::abs(quantized1 - 0.25) < 0.001);
    std::cout << "  1/16 quantize 0.3: " << quantized1 << " (expected 0.25)" << std::endl;
    
    // Test 1/8 note quantization
    double time2 = 0.6;  // Should quantize to 0.5 (1/8 of 4 beats = 0.5)
    double quantized2 = MIDIGenerator::applyQuantization(time2, Q_1_8, timeSig);
    assert(std::abs(quantized2 - 0.5) < 0.001);
    std::cout << "  1/8 quantize 0.6: " << quantized2 << " (expected 0.5)" << std::endl;
    
    // Test 1/4 note quantization
    double time3 = 1.3;  // Should quantize to 1.0 (1/4 of 4 beats = 1.0)
    double quantized3 = MIDIGenerator::applyQuantization(time3, Q_1_4, timeSig);
    assert(std::abs(quantized3 - 1.0) < 0.001);
    std::cout << "  1/4 quantize 1.3: " << quantized3 << " (expected 1.0)" << std::endl;
    
    // Test 1/2 note quantization
    double time4 = 2.3;  // Should quantize to 2.0 (1/2 of 4 beats = 2.0)
    double quantized4 = MIDIGenerator::applyQuantization(time4, Q_1_2, timeSig);
    assert(std::abs(quantized4 - 2.0) < 0.001);
    std::cout << "  1/2 quantize 2.3: " << quantized4 << " (expected 2.0)" << std::endl;
    
    // Test 1 bar quantization
    double time5 = 5.0;  // Should quantize to 4.0 (1 bar = 4 beats)
    double quantized5 = MIDIGenerator::applyQuantization(time5, Q_1_BAR, timeSig);
    assert(std::abs(quantized5 - 4.0) < 0.001);
    std::cout << "  1 bar quantize 5.0: " << quantized5 << " (expected 4.0)" << std::endl;
    
    // Test 1/32 note quantization
    double time6 = 0.15;  // Should quantize to 0.125 (1/32 of 4 beats = 0.125)
    double quantized6 = MIDIGenerator::applyQuantization(time6, Q_1_32, timeSig);
    assert(std::abs(quantized6 - 0.125) < 0.001);
    std::cout << "  1/32 quantize 0.15: " << quantized6 << " (expected 0.125)" << std::endl;
    
    // Test with different time signature (3/4)
    TimeSignature timeSig34(3, 4);  // 3/4 time, 3 beats per bar
    double time7 = 1.6;  // Should quantize to 1.5 (1/4 of 3 beats = 0.75)
    double quantized7 = MIDIGenerator::applyQuantization(time7, Q_1_4, timeSig34);
    assert(std::abs(quantized7 - 1.5) < 0.001);
    std::cout << "  3/4 time 1/4 quantize 1.6: " << quantized7 << " (expected 1.5)" << std::endl;
    
    std::cout << "  ✓ All applyQuantization tests passed" << std::endl;
}

void testCreateNoteMessages() {
    std::cout << "Testing createNoteOn() and createNoteOff()..." << std::endl;
    
    // Test note-on creation
    auto noteOn = MIDIGenerator::createNoteOn(1, 60, 100);
    assert(noteOn.isNoteOn());
    assert(noteOn.getChannel() == 1);
    assert(noteOn.getNoteNumber() == 60);
    assert(noteOn.getVelocity() == 100);
    std::cout << "  Note-on: channel=" << noteOn.getChannel() 
              << " note=" << noteOn.getNoteNumber() 
              << " velocity=" << noteOn.getVelocity() << std::endl;
    
    // Test note-off creation
    auto noteOff = MIDIGenerator::createNoteOff(2, 72);
    assert(noteOff.isNoteOff());
    assert(noteOff.getChannel() == 2);
    assert(noteOff.getNoteNumber() == 72);
    std::cout << "  Note-off: channel=" << noteOff.getChannel() 
              << " note=" << noteOff.getNoteNumber() << std::endl;
    
    std::cout << "  ✓ All MIDI message creation tests passed" << std::endl;
}

//==============================================================================
int main() {
    std::cout << "\n=== MIDIGenerator Unit Tests ===" << std::endl;
    
    try {
        testCalculateMidiNote();
        testCalculateVelocity();
        testApplyQuantization();
        testCreateNoteMessages();
        
        std::cout << "\n✓ All MIDIGenerator tests passed!" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

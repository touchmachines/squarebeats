# Task 3: Coordinate and Time Conversion Utilities Summary

## Overview
Successfully implemented coordinate and time conversion utilities for the SquareBeats VST3 plugin. These utilities provide essential functions for converting between normalized coordinates and musical time, as well as mapping spatial properties to MIDI values.

## Files Created

### 1. ConversionUtils.h
A header-only utility library containing inline functions for:

#### Time Conversion Functions
- **`normalizedToBeats()`** - Converts normalized time position (0.0 to 1.0) to beats
  - Takes into account loop length in bars and time signature
  - Returns precise beat position for playback engine
  
- **`beatsToNormalized()`** - Converts beats to normalized time position
  - Inverse of normalizedToBeats()
  - Used for displaying playback position in UI

#### MIDI Mapping Functions
- **`mapVerticalPositionToNote()`** - Maps vertical position to MIDI note number
  - Linear interpolation between high and low notes
  - Supports pitch offset from pitch sequencer
  - Clamps result to valid MIDI range (0-127)
  - Handles fractional pitch offsets with proper rounding
  
- **`mapHeightToVelocity()`** - Maps square height to MIDI velocity
  - Maps normalized height (0.0 to 1.0) to velocity range (1-127)
  - Ensures minimum velocity of 1 (MIDI spec requirement)
  - Clamps to valid range

### 2. ConversionUtils.test.cpp
Comprehensive unit tests using assert-based testing (matching project style):
- Time conversion tests for various time signatures (4/4, 3/4, 6/8)
- Round-trip conversion tests (normalized → beats → normalized)
- MIDI mapping tests with various pitch ranges and offsets
- Edge case tests (clamping, fractional offsets, inverted ranges)

### 3. ConversionUtils.standalone.test.cpp
Standalone test file that can be compiled without JUCE dependencies:
- Minimal TimeSignature struct for testing
- All conversion functions duplicated for standalone testing
- Same test coverage as main test file
- Successfully compiled and run with g++

### 4. Updated CMakeLists.txt
- Added ConversionUtils.h to plugin target sources
- Added ConversionUtilsTests target for unit testing
- Configured proper include directories and C++17 standard

## Implementation Details

### Time Conversion Algorithm
```cpp
// Normalized to beats
totalBeats = loopLengthBars × (numerator × 4 / denominator)
beats = normalized × totalBeats

// Beats to normalized
normalized = beats / totalBeats
```

### MIDI Note Mapping Algorithm
```cpp
// Linear interpolation with pitch offset
noteFloat = highNote + (lowNote - highNote) × normalizedY + pitchOffset
note = round(noteFloat)
note = clamp(note, 0, 127)
```

### Velocity Mapping Algorithm
```cpp
// Map height to velocity range 1-127
velocity = round(1 + normalizedHeight × 126)
velocity = clamp(velocity, 1, 127)
```

## Test Results

All tests pass successfully:
- ✓ normalizedToBeats() - Correct conversion for 4/4, 3/4, 6/8 time signatures
- ✓ beatsToNormalized() - Correct inverse conversion
- ✓ Time conversion round-trip - Perfect round-trip accuracy
- ✓ mapVerticalPositionToNote() - Correct mapping with and without pitch offset
- ✓ mapHeightToVelocity() - Correct velocity mapping across full range
- ✓ Edge cases - Proper clamping and handling of boundary conditions

## Requirements Satisfied

### Subtask 3.1 ✓
- ✓ normalizedToBeats() function implemented
- ✓ beatsToNormalized() function implemented
- ✓ Time signature and loop length handled in conversions
- **Requirements: 5.3, 5.4, 6.2, 6.4**

### Subtask 3.3 ✓
- ✓ mapVerticalPositionToNote() with pitch offset and clamping
- ✓ mapHeightToVelocity() with 1-127 range
- **Requirements: 3.3, 3.4, 10.3, 10.4, 10.7**

## Key Features

### Header-Only Design
- All functions are inline for zero-overhead abstraction
- No separate compilation unit needed
- Easy to use across the codebase

### Robust Input Handling
- Proper clamping to valid MIDI ranges (0-127 for notes, 1-127 for velocity)
- Handles fractional pitch offsets with correct rounding
- Works with inverted pitch ranges (high < low)
- Handles edge cases like same high/low note

### Time Signature Support
- Works with any valid time signature
- Correctly calculates beats per bar using formula: numerator × (4 / denominator)
- Supports common signatures: 4/4, 3/4, 6/8, 7/8, etc.

### Pitch Offset Integration
- Seamlessly integrates pitch sequencer offset
- Supports fractional semitones for smooth pitch modulation
- Proper rounding ensures discrete MIDI note numbers

## Usage Examples

```cpp
// Convert normalized position to beats
TimeSignature timeSig(4, 4);
int loopLength = 2;
double beats = normalizedToBeats(0.5f, loopLength, timeSig);  // Returns 4.0

// Convert beats back to normalized
float normalized = beatsToNormalized(4.0, loopLength, timeSig);  // Returns 0.5

// Map vertical position to MIDI note
int highNote = 84;  // C6
int lowNote = 48;   // C3
float pitchOffset = 2.5f;  // +2.5 semitones from pitch sequencer
int note = mapVerticalPositionToNote(0.5f, highNote, lowNote, pitchOffset);

// Map square height to velocity
int velocity = mapHeightToVelocity(0.75f);  // Returns ~95-96
```

## Integration Points

These utilities will be used by:

1. **PlaybackEngine (Task 6)**
   - Convert normalized square positions to beat times
   - Calculate MIDI note numbers and velocities during playback
   - Apply quantization to beat times

2. **UI Components (Tasks 11-14)**
   - Display playback position indicator
   - Show beat grid lines at correct positions
   - Preview MIDI note values when hovering over squares

3. **MIDI Generator (Task 4)**
   - Generate MIDI note-on/off messages with correct note numbers
   - Calculate velocities from square heights
   - Apply pitch sequencer offsets

## Next Steps

The conversion utilities are now ready for integration with:
- MIDIGenerator class (Task 4) - Will use these functions for MIDI event generation
- PlaybackEngine class (Task 6) - Will use time conversions for playback synchronization
- UI components (Tasks 11-14) - Will use conversions for visual feedback

## Notes

- All functions are thoroughly tested with comprehensive unit tests
- Implementation follows the design document specifications exactly
- Code is optimized for performance with inline functions
- No external dependencies beyond standard C++ library
- Compatible with C++17 standard used throughout the project


# Per-Color Loop Lengths Feature

## Overview

SquareBeats now supports **independent loop lengths for each color channel**. Each color can have its own loop length that runs independently, creating polyrhythmic patterns with multiple playheads scanning at different rates.

## Features

### Default Behavior
- By default, all colors use the **global loop length** (set in the top bar)
- This maintains backward compatibility with existing patterns

### Per-Color Override
- Each color can override the global loop length with its own value (1-64 bars)
- Set via the **"Loop Len:"** dropdown in the color configuration panel (SQUARES tab)
- Select "Global" to revert to using the global loop length

### Visual Feedback
- **4 colored playheads** - one for each color channel
- Each playhead moves at its own rate based on its loop length
- Playheads use the color's display color with transparency
- Motion trails behind each playhead show direction and speed

### Playback Behavior
- Each color's squares trigger based on its own loop length
- All play modes (Forward, Backward, Pendulum, Probability) work per-color
- Pitch sequencer continues to run independently with its own per-color loop length

## Use Cases

### Polyrhythmic Patterns
Create complex rhythmic relationships:
- Color 1: 4 bars (main groove)
- Color 2: 3 bars (creates 4:3 polyrhythm)
- Color 3: 5 bars (adds 4:5 relationship)
- Color 4: 2 bars (double-time feel)

### Evolving Textures
Different loop lengths cause patterns to phase in and out of sync, creating evolving musical textures that repeat on longer cycles.

### Layered Complexity
Combine with the existing pitch sequencer per-color loop lengths for even more rhythmic independence.

## Implementation Details

### Modified Files

#### Core Data Structures
- **DataStructures.h**: Added `mainLoopLengthBars` to `ColorChannelConfig` (0.0 = use global)

#### Playback Engine
- **PlaybackEngine.h/cpp**: 
  - Added per-color position tracking (`colorPositionBeats[4]`, `colorLoopLengthBeats[4]`)
  - New method: `getNormalizedPlaybackPositionForColor(colorId)`
  - New method: `processColorTriggers()` - processes squares for a specific color
  - Updated `processBlock()` to process each color independently
  - Updated `updatePlaybackPosition()` to advance all color positions

#### UI Components
- **SequencingPlaneComponent.h/cpp**:
  - Added `colorPlaybackPositions[4]` array
  - New method: `setColorPlaybackPosition(colorId, position)`
  - Updated `drawPlaybackIndicator()` to draw 4 colored playheads

- **ColorConfigPanel.h/cpp**:
  - Added "Loop Len:" dropdown control
  - Options: "Global" or 1-64 bars
  - Handler: `onMainLoopLengthChanged()`

- **PluginEditor.cpp**:
  - Updated timer callback to set per-color playback positions

#### State Management
- **StateManager.cpp**:
  - Added serialization/deserialization of `mainLoopLengthBars` for each color
  - Maintains backward compatibility (defaults to 0.0 = global)

## Technical Notes

### Position Tracking
- Global position (`currentPositionBeats`) - used for UI reference
- Per-color positions (`colorPositionBeats[4]`) - used for triggering squares
- Absolute position (`absolutePositionBeats`) - used for pitch sequencer (unchanged)

### Loop Length Resolution
Each color's loop length is resolved at playback time:
```cpp
if (config.mainLoopLengthBars > 0.0) {
    // Use per-color override
    colorLoopLengthBeats[colorId] = config.mainLoopLengthBars * beatsPerBar;
} else {
    // Use global loop length
    colorLoopLengthBeats[colorId] = loopLengthBeats;
}
```

### Play Mode Behavior
All play modes work per-color:
- **Forward**: Each color advances forward at its own rate
- **Backward**: Each color moves backward at its own rate
- **Pendulum**: Shared direction state, but each color bounces at its own loop boundaries
- **Probability**: Each color has its own step grid based on its loop length

## Testing

To test the feature:
1. Build and load the plugin in your DAW
2. Draw squares in different colors
3. Select a color in the SQUARES tab
4. Change "Loop Len:" from "Global" to a specific bar count (e.g., 2 bars)
5. Repeat for other colors with different loop lengths
6. Press play and observe multiple colored playheads moving at different rates

## Future Enhancements

Potential improvements:
- Visual indicator showing which colors are using custom loop lengths
- Ability to quickly sync all colors to global or reset overrides
- Loop length ratios display (e.g., "4:3:2" for polyrhythmic relationships)
- Per-color play mode overrides (currently shared across all colors)

## Compatibility

- **Backward Compatible**: Existing presets load with all colors using global loop length
- **State Version**: No version bump needed - new field defaults to 0.0 (global)
- **Performance**: Minimal overhead - processes each color independently but efficiently

---

**Implementation Date**: February 4, 2026
**Estimated Complexity**: Medium (10-13 hours)
**Actual Implementation**: ~2 hours

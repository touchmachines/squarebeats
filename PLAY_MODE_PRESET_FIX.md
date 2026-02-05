# Play Mode Preset Fix

## Issue
Play mode settings (Forward, Backward, Pendulum, Probability) were not being saved in presets. When loading a preset, the play mode would not be restored, causing the plugin to always default to Forward mode.

## Root Cause
The `StateManager::saveState()` and `StateManager::loadState()` functions were not serializing/deserializing the `PlayModeConfig` structure, which contains:
- `mode` - The selected play mode (Forward/Backward/Pendulum/Probability)
- `stepJumpSize` - The step jump size for Probability mode (0.0-1.0)
- `probability` - The probability percentage for Probability mode (0.0-1.0)
- `pendulumForward` - Internal state (not saved, always starts forward)

## Solution
Updated `StateManager` to version 7 with play mode serialization:

### Changes Made

1. **StateManager.h**
   - Bumped VERSION from 6 to 7
   - Added version comment for play mode configuration

2. **StateManager.cpp - saveState()**
   - Added serialization of play mode configuration after scale sequencer
   - Writes: mode (int), stepJumpSize (float), probability (float)
   - Note: pendulumForward is internal state and not saved

3. **StateManager.cpp - loadState()**
   - Added deserialization for version 7+ presets
   - Validates and clamps all values to safe ranges
   - Always initializes pendulumForward to true (forward direction)

4. **Documentation Updates**
   - Updated PRESET_SYSTEM.md to clarify play mode details are saved
   - Updated version number reference to version 7

## Backward Compatibility
- Version 7 presets can load older presets (versions 3-6)
- Older presets will use default play mode (Forward) when loaded
- New presets with play mode will only load correctly in version 7+

## Testing Recommendations
1. Save a preset with Probability mode and custom settings
2. Switch to a different play mode
3. Load the saved preset
4. Verify play mode, step jump size, and probability are restored correctly
5. Test with all four play modes (Forward, Backward, Pendulum, Probability)
6. Verify older presets still load without errors

## Files Modified
- `SquareBeats/Source/StateManager.h`
- `SquareBeats/Source/StateManager.cpp`
- `SquareBeats/docs/PRESET_SYSTEM.md`

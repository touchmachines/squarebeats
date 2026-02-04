# Scale Sequencer Save/Load Fix

## Issue
The scale sequencer configuration (enabled state and segments) was not being saved when saving the plugin state. This meant that when you:
1. Enabled the scale sequencer
2. Added segments with different keys/scales/durations
3. Saved the project

...the scale sequencer configuration would be lost when reloading.

## Root Cause
The `StateManager` was only saving:
- ✅ Scale configuration (root note and scale type) - Version 4
- ❌ Scale sequencer configuration (enabled state and segments) - **MISSING**

## Fix Applied

### 1. Updated Version Number
Changed state format version from 4 to 5 in `StateManager.h`:
```cpp
// Version 5: Scale sequencer configuration (enabled state and segments)
static constexpr uint32_t VERSION = 5;
```

### 2. Added Save Logic
In `StateManager.cpp` `saveState()` method, added code to serialize:
- `enabled` flag (bool)
- Number of segments (int)
- For each segment:
  - `rootNote` (RootNote enum → int)
  - `scaleType` (ScaleType enum → int)
  - `lengthBars` (int, 1-16)

### 3. Added Load Logic
In `StateManager.cpp` `loadState()` method, added code to deserialize:
- Reads enabled flag and segment count
- Validates segment count (0-16 max)
- For each segment:
  - Reads and validates root note (0-11)
  - Reads and validates scale type (0-15)
  - Reads and validates length in bars (1-16)
- Ensures at least one default segment exists if none loaded
- Assigns loaded config using `model.getScaleSequencer() = scaleSeqConfig`

### 4. Backward Compatibility
The fix maintains backward compatibility:
- Version 4 files (without scale sequencer data) will still load correctly
- The load code checks `if (version >= 5)` before attempting to read scale sequencer data
- If scale sequencer data is missing, the default configuration is used (disabled, one C Major 4-bar segment)

## Testing

### Manual Testing Steps
1. **Build the plugin:**
   ```cmd
   cd SquareBeats
   build_modern.bat
   ```

2. **Install the plugin:**
   ```cmd
   xcopy /E /I /Y "build\SquareBeats_artefacts\Release\VST3\SquareBeats.vst3" "C:\vst\SquareBeats.vst3"
   ```

3. **Test in DAW:**
   - Load SquareBeats in your DAW
   - Click "Scale Seq" to enable the scale sequencer
   - Add multiple segments with different keys/scales/durations
   - Draw some squares
   - **Save the project**
   - Close and reopen the project
   - **Verify:** Scale sequencer should still be enabled with all segments intact

### Expected Behavior
- ✅ Scale sequencer enabled state persists
- ✅ All segments are saved and restored
- ✅ Each segment's root note, scale type, and length are preserved
- ✅ Playback continues with the correct scale sequence after reload

## Files Modified
- `SquareBeats/Source/StateManager.h` - Updated version to 5
- `SquareBeats/Source/StateManager.cpp` - Added save/load logic for scale sequencer
- `SquareBeats/Source/StateManager.test.cpp` - Added test cases for scale sequencer persistence

## Data Structure Saved
```cpp
struct ScaleSequencerConfig {
    bool enabled;                              // Whether scale sequencing is active
    std::vector<ScaleSequenceSegment> segments; // The sequence of scale changes
};

struct ScaleSequenceSegment {
    RootNote rootNote;    // 0-11 (C through B)
    ScaleType scaleType;  // 0-15 (Chromatic, Major, Minor, etc.)
    int lengthBars;       // 1-16 bars
};
```

## Build Status
✅ Plugin builds successfully with the fix applied
✅ No compilation errors or warnings related to the changes

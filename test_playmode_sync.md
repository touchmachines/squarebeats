# Play Mode Sync Testing Guide

## Issue
When switching play modes during playback, the sequencer would go out of sync with Ableton's clock, requiring a restart to resync.

## Root Cause
The old `resetPlaybackPosition()` would reset all positions to 0.0, ignoring the current host transport position. This caused:
- Global position to jump to start of loop
- All color positions to jump to start of their loops
- Loss of sync with DAW timeline

## Solution
Modified `resetPlaybackPosition()` to:
1. **When playing**: Sync with host's current position using `absolutePositionBeats`
2. **Per-color sync**: Each color calculates its position based on its own loop length
3. **Mode-aware**: Handles backward mode correctly (end - offset)
4. **Step sync**: Recalculates step indices for probability mode
5. **When stopped**: Still resets to zero (no host position to sync with)

## Key Implementation Details

### Global Position Sync
```cpp
double hostPosition = std::fmod(absolutePositionBeats, loopLengthBeats);
currentPositionBeats = (backward_mode) ? loopLengthBeats - hostPosition : hostPosition;
```

### Per-Color Position Sync
Each color independently syncs to host position modulo its own loop length:
```cpp
double colorHostPos = std::fmod(absolutePositionBeats, colorLoopLengthBeats[colorId]);
colorPositionBeats[colorId] = (backward_mode) ? colorLoopLengthBeats[colorId] - colorHostPos : colorHostPos;
```

### Why This Works
- `absolutePositionBeats` continuously tracks host position (never wraps to main loop)
- Each color's position is calculated independently based on its loop length
- Polyrhythmic patterns stay in sync because each color maintains its phase relationship

## Testing Checklist

### Test 1: Basic Mode Switching (Single Loop Length)
1. Load plugin in Ableton
2. Set all colors to use global loop length (4 bars)
3. Draw squares in multiple colors
4. Start playback at bar 1
5. Let it play to bar 2.5
6. Switch from Forward → Backward
   - ✅ Should continue from bar 2.5 in reverse
   - ✅ Should NOT jump to bar 0
7. Switch to Pendulum
   - ✅ Should continue from current position
8. Switch to Probability
   - ✅ Should continue from current position

### Test 2: Polyrhythmic Mode Switching (Different Loop Lengths)
1. Set colors to different loop lengths:
   - Red: 2 bars
   - Blue: 4 bars
   - Green: 3 bars
   - Yellow: Global (4 bars)
2. Draw squares in all colors
3. Start playback
4. Let it play for 6 bars (so colors are at different phases)
5. Switch play modes
   - ✅ Red should be at: `6 % 2 = 0` (start of its loop)
   - ✅ Blue should be at: `6 % 4 = 2` (middle of its loop)
   - ✅ Green should be at: `6 % 3 = 0` (start of its loop)
   - ✅ Yellow should be at: `6 % 4 = 2` (middle of its loop)

### Test 3: Backward Mode Sync
1. Set global loop to 4 bars
2. Draw squares
3. Start playback in Forward mode
4. At bar 1.5, switch to Backward
   - ✅ Should start at bar 2.5 (4 - 1.5) going backward
   - ✅ Should reach bar 0 at the same time forward would reach bar 4

### Test 4: Probability Mode Step Sync
1. Set probability mode with 50% probability, 4-step jumps
2. Start playback
3. Let it play for a while
4. Switch to Forward mode
   - ✅ Should continue from current position
5. Switch back to Probability
   - ✅ Should recalculate step index correctly
   - ✅ Should not cause timing glitches

### Test 5: Stop/Start Behavior
1. Start playback
2. Let it play to bar 3
3. Stop transport
4. Switch play mode
5. Start transport again
   - ✅ Should start from bar 0 (stopped state resets to zero)
   - ✅ Should sync with host when transport starts

### Test 6: Mid-Bar Switching
1. Set quantization to 1/16 notes
2. Draw squares on 16th note grid
3. Start playback
4. Switch modes at random times (not on bar boundaries)
   - ✅ Should maintain 16th note timing
   - ✅ Should not skip or double-trigger notes
   - ✅ Should stay locked to grid

### Test 7: Long Session Test
1. Set up complex polyrhythmic pattern
2. Let it play for 32+ bars
3. Switch modes multiple times during playback
   - ✅ Should never drift from DAW clock
   - ✅ Should not accumulate timing errors
   - ✅ Should not require transport restart to resync

## Expected Behavior Summary

| Scenario | Old Behavior | New Behavior |
|----------|--------------|--------------|
| Switch mode while playing | Jump to position 0 | Stay at current host position |
| Switch mode while stopped | Reset to 0 | Reset to 0 (unchanged) |
| Polyrhythmic colors | All jump to 0 | Each syncs to its phase |
| Backward mode | Start at 0 going backward | Start at (loop - position) |
| Probability mode | Reset step to 0 | Recalculate current step |

## Success Criteria
- ✅ No timing drift when switching modes
- ✅ No need to restart transport to resync
- ✅ Polyrhythmic patterns maintain phase relationships
- ✅ Works correctly with all 4 play modes
- ✅ Works correctly with per-color loop lengths
- ✅ No stuck notes or double triggers

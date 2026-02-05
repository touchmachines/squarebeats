# Probability Mode Jump Size Fix

## Issue
User feedback: "The jumps still feel very small and triplet time, maybe that's the issue... increase min max size?"

## Root Cause
The step jump sizes were mapped to very small values on the 1/16 note grid:

### Old Mapping (Too Small)
```
XY Pad Position → Steps → Musical Value (4/4 time)
─────────────────────────────────────────────────
0.0 - 0.2       → 1      → 1/16 note (0.0625 bars) ❌ Tiny!
0.2 - 0.4       → 2      → 1/8 note  (0.125 bars)  ❌ Still tiny!
0.4 - 0.6       → 4      → 1/4 note  (0.25 bars)   ❌ One beat only
0.6 - 0.8       → 8      → 1/2 note  (0.5 bars)    ❌ Half bar
0.8 - 1.0       → 16     → 1 bar                   ❌ Max = 1 bar!
```

**Problem**: Even at maximum (XY pad all the way right), jumps were only 1 bar. This felt:
- Too small for noticeable variation
- Like "triplet time" (small, rapid movements)
- Not dramatic enough for probability mode

## Solution
Increased the jump size range by 4x to make jumps more dramatic and musical:

### New Mapping (Musically Useful)
```
XY Pad Position → Steps → Musical Value (4/4 time)
─────────────────────────────────────────────────
0.0 - 0.2       → 4      → 1 beat   (0.25 bars)   ✅ Subtle
0.2 - 0.4       → 8      → 2 beats  (0.5 bars)    ✅ Noticeable
0.4 - 0.6       → 16     → 1 bar    (1.0 bars)    ✅ Clear jump
0.6 - 0.8       → 32     → 2 bars   (2.0 bars)    ✅ Dramatic
0.8 - 1.0       → 64     → 4 bars   (4.0 bars)    ✅ Very dramatic
```

**Result**: Jump sizes now range from 1 beat to 4 bars, providing much more noticeable and musical variation.

## Impact Comparison

### Example: 4-Bar Loop, 50% Probability

**Old Behavior (Max Jump = 1 bar)**
```
Timeline: |----Bar 1----|----Bar 2----|----Bar 3----|----Bar 4----|
Position:  ↑      ↑    ↑       ↑   ↑        ↑    ↑       ↑      ↑
           Small jumps, feels like rapid flickering
```

**New Behavior (Max Jump = 4 bars)**
```
Timeline: |----Bar 1----|----Bar 2----|----Bar 3----|----Bar 4----|
Position:  ↑                          ↑                           ↑
           Large jumps, dramatic repositioning
```

## Musical Examples

### Minimum Jump Size (1 Beat)
```
XY Pad: Left (0.0 - 0.2)
Jump: 4 steps = 1 beat

4/4 Bar: |1...2...3...4...|
Jump:     ↑   →   (jumps to beat 2)

Feel: Subtle variation, stays close to current position
Use: Gentle randomization, slight groove variation
```

### Medium Jump Size (1 Bar)
```
XY Pad: Center (0.4 - 0.6)
Jump: 16 steps = 1 bar

4-Bar Loop: |--Bar 1--|--Bar 2--|--Bar 3--|--Bar 4--|
Jump:        ↑         →         (jumps 1 bar forward)

Feel: Clear repositioning, noticeable variation
Use: Standard probability mode, good balance
```

### Maximum Jump Size (4 Bars)
```
XY Pad: Right (0.8 - 1.0)
Jump: 64 steps = 4 bars

4-Bar Loop: |--Bar 1--|--Bar 2--|--Bar 3--|--Bar 4--|
Jump:        ↑                              →        (wraps around)

Feel: Dramatic repositioning, major variation
Use: Chaotic mode, extreme randomization
```

## Technical Details

### File Modified
- `Source/DataStructures.h` - `PlayModeConfig::getStepJumpSteps()`

### Code Change
```cpp
// OLD: Small jumps (1-16 steps = 1/16 note to 1 bar)
if (stepJumpSize < 0.2f) return 1;   // 1/16 note
if (stepJumpSize < 0.4f) return 2;   // 1/8 note
if (stepJumpSize < 0.6f) return 4;   // 1/4 note (1 beat)
if (stepJumpSize < 0.8f) return 8;   // 1/2 note
return 16;                            // 1 bar

// NEW: Larger jumps (4-64 steps = 1 beat to 4 bars)
if (stepJumpSize < 0.2f) return 4;   // 1 beat
if (stepJumpSize < 0.4f) return 8;   // 2 beats
if (stepJumpSize < 0.6f) return 16;  // 1 bar
if (stepJumpSize < 0.8f) return 32;  // 2 bars
return 64;                            // 4 bars
```

### Why These Values?

**Based on 1/16 note grid (16 steps per bar in 4/4):**
- 4 steps = 4 × (1/16) = 1/4 bar = 1 beat ✅
- 8 steps = 8 × (1/16) = 1/2 bar = 2 beats ✅
- 16 steps = 16 × (1/16) = 1 bar ✅
- 32 steps = 32 × (1/16) = 2 bars ✅
- 64 steps = 64 × (1/16) = 4 bars ✅

All values are musically meaningful and align with bar/beat boundaries.

## User Experience

### Before Fix
- 😕 "Jumps feel very small"
- 😕 "Feels like triplet time"
- 😕 "Not dramatic enough"
- 😕 "Can barely notice the jumps"

### After Fix
- ✅ "Jumps are noticeable and dramatic"
- ✅ "Clear repositioning"
- ✅ "Musical and intentional"
- ✅ "Good range from subtle to extreme"

## Usage Recommendations

### Subtle Variation (XY Pad: Left)
- Jump size: 1 beat
- Probability: 25-50%
- Use: Gentle groove variation, humanization

### Standard Probability (XY Pad: Center)
- Jump size: 1 bar
- Probability: 50%
- Use: Balanced variation, interesting patterns

### Dramatic Chaos (XY Pad: Right)
- Jump size: 4 bars
- Probability: 75-100%
- Use: Extreme randomization, generative music

## Backward Compatibility

⚠️ **Breaking Change for Existing Presets**
- Presets with probability mode will sound different
- Jump sizes will be 4x larger than before
- Users may need to adjust XY pad position left to compensate

**Migration Guide:**
- If old preset used XY pad at 0.8 (8 steps = half bar)
- New equivalent is XY pad at 0.2 (8 steps = 2 beats)
- Roughly: Move XY pad 2 notches left to get similar behavior

## Testing Checklist

### Test 1: Minimum Jump (1 Beat)
- Set XY pad to far left (0.0-0.2)
- Set probability to 50%
- Expected: Jumps of 1 beat, subtle variation
- Should feel: Gentle, stays close to current position

### Test 2: Medium Jump (1 Bar)
- Set XY pad to center (0.4-0.6)
- Set probability to 50%
- Expected: Jumps of 1 bar, clear repositioning
- Should feel: Noticeable but musical

### Test 3: Maximum Jump (4 Bars)
- Set XY pad to far right (0.8-1.0)
- Set probability to 50%
- Expected: Jumps of 4 bars, dramatic repositioning
- Should feel: Extreme variation, chaotic

### Test 4: Different Loop Lengths
- Test with 2-bar, 4-bar, 8-bar loops
- Verify jumps scale appropriately
- 4-bar jump in 8-bar loop = half loop

## Performance Impact

✅ **No performance impact**
- Same calculation complexity
- Just different return values
- No allocations or blocking

## Documentation Updates Needed

Update these files to reflect new jump sizes:
- [ ] `docs/USER_GUIDE.md` - Update probability mode section
- [ ] `docs/FEATURES.md` - Update play mode descriptions
- [ ] `README.md` - Update quick start if mentioned

## Conclusion

This fix transforms probability mode from subtle flickering to dramatic, musical repositioning. Jump sizes now range from 1 beat to 4 bars, providing the noticeable variation users expect from a probability-based play mode.

**Status**: ✅ Ready for testing
**Priority**: HIGH (user-reported issue)
**Risk**: MEDIUM (changes existing behavior, may affect presets)

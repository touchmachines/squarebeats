# Probability Mode Fix - Musical Step Sizes & Per-Color Independence

## Problems Fixed

### 1. Global Step Tracking
Previously, probability mode used global step tracking, so all colors would jump together when a probability event occurred. This defeated the purpose of having independent loop lengths per color.

### 2. Chaotic Step Sizes
The old implementation mapped the step jump size linearly from 1-16 steps:
- `stepJumpSize = 0.0` → 1 step
- `stepJumpSize = 0.5` → 8 steps  
- `stepJumpSize = 1.0` → 16 steps

This created **musically awkward jumps** at odd divisions like 3, 5, 7, 9, 11, 13, 15 steps that don't align with musical timing.

### 3. Random Direction
The old code randomly chose forward or backward jumps, making the pattern even more unpredictable and chaotic.

## Solutions Implemented

### 1. Per-Color Step Tracking
Added `int colorCurrentStep[4]` to track each color's step position independently. Now each color:
- Advances at its own pace based on its loop length
- Makes probability jumps independently
- Creates polyrhythmic probability patterns

### 2. Musical Step Divisions
Changed `getStepJumpSteps()` to return only **powers of 2** for musical timing:

```cpp
int getStepJumpSteps() const {
    // Map to 5 musical divisions: 1, 2, 4, 8, 16 steps
    if (stepJumpSize < 0.2f) return 1;   // 0.0 - 0.2 -> 1 step
    if (stepJumpSize < 0.4f) return 2;   // 0.2 - 0.4 -> 2 steps
    if (stepJumpSize < 0.6f) return 4;   // 0.4 - 0.6 -> 4 steps
    if (stepJumpSize < 0.8f) return 8;   // 0.6 - 0.8 -> 8 steps
    return 16;                            // 0.8 - 1.0 -> 16 steps
}
```

**XY Pad Mapping:**
- **Left side (X = 0.0-0.2)**: 1-step jumps (subtle variation)
- **Left-center (X = 0.2-0.4)**: 2-step jumps (half-bar in 16th notes)
- **Center (X = 0.4-0.6)**: 4-step jumps (quarter-bar)
- **Right-center (X = 0.6-0.8)**: 8-step jumps (half-bar)
- **Right side (X = 0.8-1.0)**: 16-step jumps (full bar)

These divisions align perfectly with musical timing and create predictable, groove-oriented patterns.

### 3. Forward-Only Jumps
Changed from random direction to **always jumping forward**. This creates:
- More predictable patterns
- Forward momentum in the sequence
- Easier to understand what's happening musically

If a jump would go past the end of the loop, it wraps around (modulo operation).

## Implementation Details

### Changes Made

1. **DataStructures.h**
   - Updated `getStepJumpSteps()` to return musical divisions (1, 2, 4, 8, 16)
   - Updated comments to reflect new mapping

2. **PlaybackEngine.h**
   - Added `int colorCurrentStep[4]` for per-color step tracking

3. **PlaybackEngine.cpp**
   - Initialize `colorCurrentStep[i] = 0` in constructor
   - Reset per-color step indices when transport stops
   - Reset per-color step indices in `resetPlaybackPosition()`
   - **Rewrote PLAY_PROBABILITY case** in `updatePlaybackPosition()`:
     ```cpp
     case PLAY_PROBABILITY:
         // Each color advances normally and may jump forward on step boundaries
         {
             // Calculate steps for this color's loop
             int colorSteps = std::max(1, static_cast<int>(colorLoopBeats / (timeSig.getBeatsPerBar() / 16.0)));
             double beatsPerColorStep = colorLoopBeats / colorSteps;
             int previousColorStep = colorCurrentStep[colorId];
             
             // Advance position normally
             colorPositionBeats[colorId] += beatsElapsed;
             if (colorPositionBeats[colorId] >= colorLoopBeats) {
                 colorPositionBeats[colorId] = std::fmod(colorPositionBeats[colorId], colorLoopBeats);
             }
             
             // Calculate current step
             int newColorStep = static_cast<int>(colorPositionBeats[colorId] / beatsPerColorStep) % colorSteps;
             
             // Check if we crossed a step boundary
             if (newColorStep != previousColorStep) {
                 // Roll for probability jump
                 if (randomGenerator.nextFloat() < playModeConfig.probability) {
                     // Jump forward by musical step size (1, 2, 4, 8, or 16 steps)
                     int jumpSteps = playModeConfig.getStepJumpSteps();
                     colorCurrentStep[colorId] = (colorCurrentStep[colorId] + jumpSteps) % colorSteps;
                     
                     // Update position to match new step
                     colorPositionBeats[colorId] = colorCurrentStep[colorId] * beatsPerColorStep;
                 } else {
                     // Normal advance
                     colorCurrentStep[colorId] = newColorStep;
                 }
             }
         }
         break;
     ```

## Behavior

Now in probability mode:
- Each color advances independently through its own loop
- On each step boundary, there's a chance (Y-axis) of jumping forward
- Jumps are always forward by a musical division (X-axis: 1, 2, 4, 8, or 16 steps)
- Different colors can have different loop lengths and jump independently
- Creates **musical, groove-oriented probability patterns** instead of chaos

## XY Pad Controls

- **X-axis (Step Jump Size)**: How far to jump when a probability event occurs
  - Far left: 1 step (subtle)
  - Center: 4 steps (quarter-bar)
  - Far right: 16 steps (full bar)
  
- **Y-axis (Probability)**: How often jumps occur
  - Bottom: 0% (never jump, normal playback)
  - Center: 50% (jump half the time)
  - Top: 100% (always jump)

## Testing

To test the improvements:
1. Set different loop lengths for different colors
2. Select Probability mode (--?>)
3. Adjust the XY pad:
   - Try X=center, Y=0.5 for moderate 4-step jumps
   - Try X=right, Y=0.3 for occasional full-bar jumps
   - Try X=left, Y=0.8 for frequent subtle variations
4. Notice how each color jumps independently
5. Notice how jumps stay musically aligned (no odd divisions)

## Date

February 4, 2026

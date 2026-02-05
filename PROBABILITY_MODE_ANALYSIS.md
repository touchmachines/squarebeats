# Probability Mode Analysis - "Jumps Feel Off-Time"

## Issue Description

User reports: "The jumps feel off time, a little wild where I am intending to bounce around the read position in a quantized way."

## Root Cause Analysis

### Current Implementation Problems

#### 1. Step Grid Too Fine (1/16 Notes)
```cpp
// Current: Uses 1/16 notes as step grid
double stepInterval = beatsPerBar / 16.0;
// For 4-bar loop in 4/4: 64 steps total
```

**Problem**: With 64 steps in a 4-bar loop, the system checks for jumps 64 times per loop cycle.

#### 2. Too Many Jump Opportunities
```cpp
// Checks EVERY step boundary
if (newStep != previousStep) {
    if (randomGenerator.nextFloat() < playModeConfig.probability) {
        // Jump happens!
    }
}
```

**At 50% probability in a 4-bar loop**:
- 64 step boundaries
- ~32 jumps per loop
- Jump every ~0.125 bars on average
- **Result**: Feels chaotic, not musical

#### 3. Jumps Not Quantized to Musical Boundaries

Jumps can happen on any 1/16 note boundary, not just beats or bars.

**Example Timeline** (4/4 time, 50% probability):
```
Bar 1:    |1...2...3...4...|
Jumps:     ↑ ↑   ↑ ↑ ↑   ↑ ↑  (7 jumps in one bar!)
```

This feels "wild" and "off-time" because:
- Too frequent
- Not aligned with strong beats
- Unpredictable timing

## Comparison: What User Expects vs What Happens

### User Expectation
```
"Bounce around the read position in a quantized way"

Desired behavior:
- Jumps on beat boundaries (1, 2, 3, 4)
- Or bar boundaries (1, 5, 9, 13)
- Predictable, musical timing
- Controlled variation

Example (50% probability, beat quantization):
Bar 1:    |1...2...3...4...|
Jumps:     ↑       ↑         (2 jumps, on beats)
```

### Current Reality
```
Actual behavior:
- Jumps on 1/16 note boundaries
- 64 opportunities per 4-bar loop
- ~32 jumps at 50% probability
- Feels random and chaotic

Example (50% probability, 1/16 quantization):
Bar 1:    |1...2...3...4...|
Jumps:     ↑↑ ↑  ↑↑ ↑↑  ↑   (8 jumps in one bar!)
```

## Mathematical Analysis

### Current System (1/16 Note Grid)

**4-bar loop in 4/4 time:**
- Loop length: 16 beats
- Step interval: 0.25 beats (1/16 note)
- Total steps: 64
- Jump checks per loop: 64

**At different probabilities:**
- 25% probability: ~16 jumps per loop (1 per beat)
- 50% probability: ~32 jumps per loop (2 per beat)
- 75% probability: ~48 jumps per loop (3 per beat)

**Problem**: Even at 25%, you get 1 jump per beat = too chaotic!

### Proposed System (Beat Quantization)

**4-bar loop in 4/4 time:**
- Loop length: 16 beats
- Jump check interval: 1 beat
- Total jump checks: 16
- Jump checks per loop: 16

**At different probabilities:**
- 25% probability: ~4 jumps per loop (1 per bar)
- 50% probability: ~8 jumps per loop (2 per bar)
- 75% probability: ~12 jumps per loop (3 per bar)

**Result**: Much more musical and controlled!

## Proposed Solutions

### Solution 1: Beat-Quantized Jumps (RECOMMENDED)

Check for jumps only on beat boundaries (quarter notes in 4/4).

**Benefits:**
- Jumps feel "on time"
- Predictable, musical timing
- Still provides variation
- Matches user expectation

**Implementation:**
```cpp
// Check for jumps only on beat boundaries
double beatInterval = 1.0; // One beat
int beatsInLoop = static_cast<int>(loopLengthBeats);
int currentBeat = static_cast<int>(currentPositionBeats);
int previousBeat = static_cast<int>(previousPositionBeats);

if (currentBeat != previousBeat) {
    // We crossed a beat boundary - check for jump
    if (randomGenerator.nextFloat() < playModeConfig.probability) {
        // Jump!
    }
}
```

### Solution 2: Bar-Quantized Jumps

Check for jumps only on bar boundaries.

**Benefits:**
- Even more controlled
- Very predictable
- Good for longer loops

**Trade-off**: Less variation, might feel too rigid

### Solution 3: Configurable Jump Quantization

Add a parameter to control jump quantization:
- 1/4 note (beat)
- 1/2 note
- 1 bar
- 2 bars

**Benefits:**
- Maximum flexibility
- User can dial in desired feel
- Different settings for different musical contexts

**Trade-off**: More complexity

## Recommended Approach

**Implement Solution 1 (Beat-Quantized Jumps) as default**, with these characteristics:

1. **Jump Check Interval**: 1 beat (quarter note in 4/4)
2. **Keep existing step jump sizes**: 1, 2, 4, 8, 16 steps
3. **Keep existing probability control**: 0-100%

This gives:
- Musical, on-time feel
- Controlled variation
- Predictable behavior
- Matches user expectation of "quantized" jumps

## Example Scenarios

### Scenario 1: Subtle Variation (25% probability, 2-step jumps)
```
4-bar loop, beat quantization:
- 16 beat boundaries
- ~4 jumps per loop
- Jump size: 2 steps (half bar)

Timeline:
Bar 1:    |1...2...3...4...|
          ↑               
Bar 2:    |1...2...3...4...|
                      ↑     
Bar 3:    |1...2...3...4...|
              ↑             
Bar 4:    |1...2...3...4...|
                          ↑ 

Result: Subtle, musical variation
```

### Scenario 2: Moderate Chaos (50% probability, 4-step jumps)
```
4-bar loop, beat quantization:
- 16 beat boundaries
- ~8 jumps per loop
- Jump size: 4 steps (1 bar)

Timeline:
Bar 1:    |1...2...3...4...|
          ↑       ↑         
Bar 2:    |1...2...3...4...|
              ↑       ↑     
Bar 3:    |1...2...3...4...|
          ↑           ↑     
Bar 4:    |1...2...3...4...|
                  ↑       ↑ 

Result: Interesting variation, still musical
```

### Scenario 3: High Variation (75% probability, 8-step jumps)
```
4-bar loop, beat quantization:
- 16 beat boundaries
- ~12 jumps per loop
- Jump size: 8 steps (2 bars)

Timeline:
Bar 1:    |1...2...3...4...|
          ↑   ↑   ↑         
Bar 2:    |1...2...3...4...|
          ↑       ↑   ↑     
Bar 3:    |1...2...3...4...|
              ↑   ↑   ↑     
Bar 4:    |1...2...3...4...|
          ↑   ↑       ↑     

Result: High variation, but still on-beat
```

## Implementation Plan

1. **Change jump check interval** from 1/16 notes to beats
2. **Keep step grid at 1/16 notes** for position tracking (fine resolution)
3. **Only check for jumps on beat boundaries** (coarse resolution)
4. **Test with different probabilities** to verify musical feel

## Expected Outcome

After fix:
- ✅ Jumps feel "on time" (quantized to beats)
- ✅ Controlled variation (not chaotic)
- ✅ Predictable behavior
- ✅ Musical and intentional
- ✅ Matches user expectation

## Code Changes Required

**File**: `PlaybackEngine.cpp`
**Function**: `updatePlaybackPosition()` - PLAY_PROBABILITY case

**Change**: Check for jumps on beat boundaries instead of step boundaries.

**Estimated Impact**: Low risk, isolated change, easy to test.

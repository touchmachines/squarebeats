# Pendulum Mode Fix - Independent Per-Color Direction

## Problem

Previously, pendulum mode used a **global `pendulumForward` state** that controlled the direction for all color channels simultaneously. When the global read head hit the end of its loop and reversed direction, all color channels would reverse together - even though each color has its own independent loop length.

This meant that a color with a shorter loop would be forced to reverse direction when the global loop reversed, rather than reversing at its own loop boundary.

## Solution

Added **per-color pendulum direction tracking** so each color channel maintains its own forward/backward state based on its own loop length.

### Changes Made

1. **PlaybackEngine.h**
   - Added `bool colorPendulumForward[4]` array to track each color's pendulum direction
   - Updated comment for global `pendulumForward` to clarify it's for global position tracking only

2. **PlaybackEngine.cpp**
   - Initialize `colorPendulumForward[i] = true` in constructor
   - Reset per-color pendulum directions when transport stops
   - Reset per-color pendulum directions in `resetPlaybackPosition()`
   - **Critical fix in `updatePlaybackPosition()`**: Each color now checks its own loop boundary and flips its own direction:
     ```cpp
     case PLAY_PENDULUM:
         // Each color has its own pendulum direction based on its own loop length
         if (colorPendulumForward[colorId]) {
             colorPositionBeats[colorId] += beatsElapsed;
             if (colorPositionBeats[colorId] >= colorLoopBeats) {
                 // Hit the end - bounce back
                 colorPositionBeats[colorId] = colorLoopBeats - (colorPositionBeats[colorId] - colorLoopBeats);
                 colorPendulumForward[colorId] = false;
             }
         } else {
             colorPositionBeats[colorId] -= beatsElapsed;
             if (colorPositionBeats[colorId] <= 0.0) {
                 // Hit the start - bounce forward
                 colorPositionBeats[colorId] = -colorPositionBeats[colorId];
                 colorPendulumForward[colorId] = true;
             }
         }
         break;
     ```

## Behavior

Now in pendulum mode:
- Each color channel bounces back and forth within its own loop length
- A color with a 2-bar loop will reverse every 2 bars
- A color with a 4-bar loop will reverse every 4 bars
- They operate completely independently, creating interesting polyrhythmic patterns

## Testing

To test the fix:
1. Set different loop lengths for different colors (e.g., Red = 2 bars, Blue = 4 bars)
2. Select Pendulum mode (<-->)
3. Start playback
4. Observe that each color's read head reverses at its own loop boundary
5. The red read head should reverse twice as often as the blue read head

## Date

February 4, 2026

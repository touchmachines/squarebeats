# SquareBeats VST3 Plugin - Testing Results

## Testing Status

This document tracks the testing status of SquareBeats VST3 plugin. Complete the checklist items as you test the plugin in your DAW.

**Date:** _____________  
**Tester:** _____________  
**DAW:** _____________  
**DAW Version:** _____________  
**OS:** _____________  
**Plugin Version:** 1.0.0  

---

## Build and Installation

- [ ] Plugin builds successfully without errors
- [ ] Plugin installs to system VST3 folder
- [ ] DAW recognizes the plugin after rescan
- [ ] Plugin UI loads without crashes

**Notes:**
```
[Add any build/installation issues here]
```

---

## 1. Plugin Loading and UI

- [ ] Plugin loads in DAW without errors
- [ ] Plugin UI appears correctly
- [ ] Minimum window size (800x600) is enforced
- [ ] Window can be resized up to maximum (2000x1500)
- [ ] UI scales proportionally when resized
- [ ] All UI components are visible and properly laid out

**Notes:**
```
[Add any UI issues here]
```

---

## 2. MIDI Output Routing

- [ ] MIDI is routed to downstream instrument
- [ ] Created squares generate MIDI notes
- [ ] MIDI notes are audible through instrument
- [ ] MIDI monitor shows correct note-on/off messages

**Notes:**
```
[Add any MIDI routing issues here]
```

---

## 3. Tempo Synchronization

- [ ] Playback syncs with DAW tempo at 120 BPM
- [ ] Playback adjusts when tempo changes to 90 BPM
- [ ] Playback adjusts when tempo changes to 140 BPM
- [ ] Tempo automation works correctly (if tested)
- [ ] Playback position indicator moves smoothly

**Notes:**
```
[Add any tempo sync issues here]
```

---

## 4. Square Drawing and Editing

- [ ] Click and drag creates a new square
- [ ] Square appears with selected color
- [ ] Clicking and dragging moves existing square
- [ ] Dragging square edge resizes it
- [ ] Delete key removes selected square
- [ ] Right-click menu deletes square (if implemented)
- [ ] Squares maintain normalized coordinates

**Notes:**
```
[Add any drawing/editing issues here]
```

---

## 5. Color Channel Configuration

- [ ] 4 color channels are available
- [ ] Selecting color channel updates UI
- [ ] Squares created with different colors
- [ ] Each color generates MIDI on assigned channel
- [ ] Quantization changes affect note timing
- [ ] Pitch range changes affect note pitch
- [ ] MIDI channel assignment routes correctly
- [ ] "Clear All" button removes all squares of selected color

**Notes:**
```
[Add any color channel issues here]
```

---

## 6. Loop Length and Time Signature

- [ ] Default loop length is 2 bars
- [ ] Loop length can be changed to 1 bar
- [ ] Loop length can be changed to 4 bars
- [ ] Loop resets at correct boundary
- [ ] Squares are preserved when loop length increases
- [ ] Squares beyond new length are hidden when loop decreases
- [ ] Default time signature is 4/4
- [ ] Time signature can be changed to 3/4
- [ ] Time signature can be changed to other values
- [ ] Bar lengths adjust correctly with time signature

**Notes:**
```
[Add any loop/time signature issues here]
```

---

## 7. Pitch Sequencer

- [ ] Pitch sequencer toggle button works
- [ ] Pitch sequencer overlay appears when visible
- [ ] Drawing on pitch sequencer records waveform
- [ ] Pitch offset is applied to generated notes
- [ ] Hiding pitch sequencer sets offset to zero
- [ ] Pitch sequencer has independent loop length

**Notes:**
```
[Add any pitch sequencer issues here]
```

---

## 8. Preset Saving and Loading

- [ ] Pattern can be saved as preset in DAW
- [ ] Saved preset appears in DAW preset list
- [ ] Loading preset restores all squares
- [ ] Loading preset restores color configurations
- [ ] Loading preset restores loop length
- [ ] Loading preset restores time signature
- [ ] Loading preset restores pitch sequencer state

**Notes:**
```
[Add any preset issues here]
```

---

## 9. Transport Control

- [ ] Starting playback begins pattern playback
- [ ] Playback position indicator moves during playback
- [ ] Stopping playback stops all active notes
- [ ] No stuck notes after stopping
- [ ] Starting from middle of loop syncs correctly
- [ ] Loop boundary is respected during playback

**Notes:**
```
[Add any transport issues here]
```

---

## 10. Monophonic Voice Behavior

- [ ] Overlapping squares of same color play monophonically
- [ ] Note-off sent before new note-on for same color
- [ ] Different colors play independently (polyphonic)
- [ ] No stuck notes with overlapping squares

**Notes:**
```
[Add any voice management issues here]
```

---

## 11. Edge Cases and Stress Testing

- [ ] Square spanning loop boundary works correctly
- [ ] Many squares (50+) perform smoothly
- [ ] Minimum window size remains usable
- [ ] Maximum window size works correctly
- [ ] Invalid host tempo defaults to 120 BPM
- [ ] Zero-width/height squares are prevented
- [ ] Coordinates are clamped to valid range [0, 1]

**Notes:**
```
[Add any edge case issues here]
```

---

## 12. Cross-Platform Compatibility (if applicable)

- [ ] Preset saved on Windows loads on macOS
- [ ] Preset saved on macOS loads on Windows
- [ ] No data corruption across platforms
- [ ] UI appears correctly on both platforms

**Notes:**
```
[Add any cross-platform issues here]
```

---

## Performance Metrics

**CPU Usage:** ______% (with 50 squares at 120 BPM)  
**Memory Usage:** ______ MB  
**UI Frame Rate:** ______ FPS  
**Latency:** ______ ms (DAW buffer size dependent)  

---

## Issues Found

### Critical Issues (Prevent Release)
```
[List any critical bugs that must be fixed before release]
```

### Major Issues (Should Fix)
```
[List any major bugs that should be fixed]
```

### Minor Issues (Nice to Fix)
```
[List any minor bugs or improvements]
```

---

## Overall Assessment

**Plugin Stability:** [ ] Excellent  [ ] Good  [ ] Fair  [ ] Poor  
**Feature Completeness:** [ ] Complete  [ ] Mostly Complete  [ ] Incomplete  
**Performance:** [ ] Excellent  [ ] Good  [ ] Fair  [ ] Poor  
**User Experience:** [ ] Excellent  [ ] Good  [ ] Fair  [ ] Poor  

**Ready for Release?** [ ] Yes  [ ] No  [ ] With Fixes  

**Additional Comments:**
```
[Add any additional comments or observations]
```

---

## Sign-off

**Tester Signature:** _____________  
**Date:** _____________  

---

## Appendix: Test Environment Details

**Hardware:**
- CPU: _____________
- RAM: _____________
- GPU: _____________

**Software:**
- OS Version: _____________
- DAW Version: _____________
- Plugin Version: _____________

**Test Instruments Used:**
```
[List any software instruments used for testing MIDI output]
```

**MIDI Monitor Tool:**
```
[Name of MIDI monitoring tool used, if any]
```

# Pitch Length "Global" Option Update

## Summary
Added "Global" as the first option in the pitch sequencer loop length dropdown to match the behavior of the main loop length dropdown. This makes the interface more consistent and intuitive.

## Changes Made

### 1. Data Structure (`DataStructures.h`)
- Updated `pitchSeqLoopLengthBars` comment to indicate `0 = use global, 1-64 = bars`
- Changed default value from `2` to `0` (use global)

### 2. UI Component (`ColorConfigPanel.cpp`)
- Added "Global" as ID 1 in the pitch sequencer length dropdown
- Shifted all bar options to IDs 2-65 (mapping to 1-64 bars)
- Updated `refreshFromModel()` to handle the new ID mapping
- Updated `onPitchSeqLengthChanged()` to convert ID 1 to value 0 (global)

### 3. Playback Engine (`PlaybackEngine.cpp`)
- Updated 4 locations where `pitchSeqLoopLengthBars` is used
- Added logic to use global loop length when value is 0:
  ```cpp
  double pitchSeqLoopBars = (config.pitchSeqLoopLengthBars > 0) 
      ? config.pitchSeqLoopLengthBars 
      : pattern->getLoopLengthBars();
  ```
- Locations updated:
  - `getNormalizedPitchSeqPosition()`
  - `processColorTriggers()` (note-on with pitch offset)
  - Probability mode note triggering
  - `sendNoteOn()` function

### 4. State Management (`StateManager.cpp`)
- Updated `juce::jlimit()` range from `(1, 64)` to `(0, 64)` to allow 0 value

### 5. Preset Manager (`PresetManager.cpp`)
- Changed default in `_Init` preset from `1` to `0` (use global)

### 6. Documentation
- Updated `ARCHITECTURE.md` to reflect new value range
- Updated `USER_GUIDE.md` with "Global" option explanation
- Updated `FEATURES.md` to mention "Global" option

## Behavior

### Before
- Pitch sequencer length: 1-64 bars only
- Each color had independent pitch sequencer timing
- No way to sync pitch sequencer with global loop length

### After
- Pitch sequencer length: "Global" or 1-64 bars
- "Global" option uses the global loop length (same as main pattern)
- Consistent with main loop length dropdown behavior
- Default is now "Global" for new patterns

## Benefits
1. **Consistency**: Matches the main loop length dropdown interface
2. **Simplicity**: Default behavior now syncs pitch with main pattern
3. **Flexibility**: Still allows independent pitch sequencer timing when needed
4. **Intuitive**: Users expect "Global" option based on main loop length behavior

## Testing Recommendations
1. Create new pattern - verify pitch length defaults to "Global"
2. Change global loop length - verify pitch sequencer follows when set to "Global"
3. Set pitch length to specific bars - verify it runs independently
4. Load old presets - verify they still work (values 1-64 are preserved)
5. Save and load new presets with "Global" setting
6. Test all 4 color channels independently

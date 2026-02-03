# SquareBeats VST3 Plugin - Build and Testing Guide

## Building the Plugin

### Prerequisites

1. **JUCE Framework**: Download and install JUCE from https://juce.com/
2. **C++ Compiler**:
   - Windows: Visual Studio 2019 or later
   - macOS: Xcode 12 or later
3. **CMake** (version 3.15 or later) - Optional, for CMake build

### Build Method 1: Using CMake (Recommended)

1. Open a terminal/command prompt in the SquareBeats directory

2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure CMake (replace `/path/to/JUCE` with your JUCE installation path):
   ```bash
   cmake -DJUCE_DIR=/path/to/JUCE ..
   ```
   
   On Windows with Visual Studio:
   ```cmd
   cmake -DJUCE_DIR=C:\JUCE -G "Visual Studio 17 2022" ..
   ```

4. Build the plugin:
   ```bash
   cmake --build . --config Release
   ```

5. The VST3 plugin will be located in:
   - Windows: `build\SquareBeats_artefacts\Release\VST3\SquareBeats.vst3`
   - macOS: `build/SquareBeats_artefacts/Release/VST3/SquareBeats.vst3`

### Build Method 2: Using Projucer

1. Open `SquareBeats.jucer` in Projucer

2. Update the .jucer file to include all source files:
   - PatternModel.cpp/h
   - MIDIGenerator.cpp/h
   - PlaybackEngine.cpp/h
   - StateManager.cpp/h
   - ConversionUtils.h
   - SequencingPlaneComponent.cpp/h
   - ColorSelectorComponent.cpp/h
   - ColorConfigPanel.cpp/h
   - LoopLengthSelector.cpp/h
   - TimeSignatureControls.cpp/h
   - ControlButtons.cpp/h
   - PitchSequencerComponent.cpp/h

3. Set the JUCE modules path in Projucer settings

4. Click "Save Project and Open in IDE"

5. Build the project in your IDE:
   - Windows: Build in Visual Studio (Release configuration)
   - macOS: Build in Xcode (Release scheme)

### Installing the Plugin

After building, copy the VST3 plugin to your system's VST3 folder:

**Windows:**
```
C:\Program Files\Common Files\VST3\SquareBeats.vst3
```

**macOS:**
```
/Library/Audio/Plug-Ins/VST3/SquareBeats.vst3
```
or
```
~/Library/Audio/Plug-Ins/VST3/SquareBeats.vst3
```

## Testing in a DAW

### Supported DAWs

SquareBeats has been designed to work with any VST3-compatible DAW, including:
- Ableton Live 10+
- FL Studio 20+
- Reaper 6+
- Cubase 11+
- Studio One 5+
- Bitwig Studio 3+

### Testing Checklist

#### 1. Plugin Loading
- [ ] Open your DAW
- [ ] Rescan VST3 plugins (if necessary)
- [ ] Create a new MIDI track
- [ ] Load SquareBeats as a MIDI effect/instrument
- [ ] Verify the plugin UI appears correctly
- [ ] Verify minimum window size (800x600) is enforced
- [ ] Test window resizing up to maximum size (2000x1500)

#### 2. MIDI Output Routing
- [ ] Add a software instrument after SquareBeats in the signal chain
- [ ] Verify MIDI is being routed to the instrument
- [ ] Create a square on the sequencing plane
- [ ] Start playback and verify you hear sound from the instrument
- [ ] Check that MIDI notes are being generated (use MIDI monitor if available)

#### 3. Tempo Synchronization
- [ ] Set DAW tempo to 120 BPM
- [ ] Create a square at the start of the loop
- [ ] Start playback and verify timing is correct
- [ ] Change DAW tempo to 90 BPM
- [ ] Verify playback speed adjusts accordingly (notes trigger at correct times)
- [ ] Change DAW tempo to 140 BPM
- [ ] Verify playback speed adjusts accordingly
- [ ] Test with tempo automation (if DAW supports it)

#### 4. Square Drawing and Editing
- [ ] Click and drag to create a square
- [ ] Verify square appears with the selected color
- [ ] Click and drag an existing square to move it
- [ ] Verify square moves to new position
- [ ] Click and drag a square edge to resize it
- [ ] Verify square resizes correctly
- [ ] Delete a square (right-click or delete key)
- [ ] Verify square is removed

#### 5. Color Channel Configuration
- [ ] Select different color channels (4 colors available)
- [ ] Create squares with different colors
- [ ] Verify each color generates MIDI on its assigned channel
- [ ] Change quantization for a color channel
- [ ] Verify notes are quantized correctly
- [ ] Change pitch range (high/low notes) for a color channel
- [ ] Verify pitch mapping changes accordingly
- [ ] Change MIDI channel assignment
- [ ] Verify MIDI is routed to the new channel

#### 6. Loop Length and Time Signature
- [ ] Change loop length from 2 bars to 1 bar
- [ ] Verify loop resets at correct time
- [ ] Change loop length to 4 bars
- [ ] Verify loop extends correctly
- [ ] Change time signature from 4/4 to 3/4
- [ ] Verify bar lengths adjust correctly
- [ ] Test with other time signatures (5/4, 7/8, etc.)

#### 7. Pitch Sequencer
- [ ] Toggle pitch sequencer visibility
- [ ] Verify pitch sequencer overlay appears/disappears
- [ ] Draw a pitch modulation curve
- [ ] Verify pitch offset is applied to generated notes
- [ ] Hide pitch sequencer
- [ ] Verify pitch offset returns to zero

#### 8. Preset Saving/Loading
- [ ] Create a pattern with multiple squares
- [ ] Configure color channels with custom settings
- [ ] Save the preset using DAW's preset system
- [ ] Clear the pattern (delete all squares)
- [ ] Load the saved preset
- [ ] Verify all squares are restored correctly
- [ ] Verify all color configurations are restored
- [ ] Verify loop length and time signature are restored

#### 9. Transport Control
- [ ] Start playback
- [ ] Verify playback position indicator moves
- [ ] Stop playback
- [ ] Verify all active notes are stopped (no stuck notes)
- [ ] Start playback from middle of loop
- [ ] Verify playback position syncs correctly

#### 10. Monophonic Voice Behavior
- [ ] Create two overlapping squares of the same color
- [ ] Start playback
- [ ] Verify only one note plays at a time per color
- [ ] Verify note-off is sent before new note-on
- [ ] Create overlapping squares of different colors
- [ ] Verify each color plays independently

#### 11. Edge Cases
- [ ] Create a square that spans the loop boundary
- [ ] Verify note-off is sent at loop end and note-on at loop start
- [ ] Create many squares (50+)
- [ ] Verify performance remains smooth
- [ ] Resize window to minimum size
- [ ] Verify UI remains usable
- [ ] Test with invalid host tempo (if possible)
- [ ] Verify plugin defaults to 120 BPM

#### 12. Cross-Platform Testing (if applicable)
- [ ] Save a preset on Windows
- [ ] Load the same preset on macOS (or vice versa)
- [ ] Verify pattern loads correctly
- [ ] Verify no data corruption

### Known Limitations

1. **MIDI Effect vs Instrument**: Some DAWs treat MIDI effects differently than instruments. If you don't see MIDI output, try:
   - Loading SquareBeats on a MIDI track (not audio track)
   - Routing the MIDI output explicitly to an instrument
   - Checking DAW-specific MIDI routing settings

2. **Preset Compatibility**: Presets are saved by the DAW, not the plugin. Preset file formats vary by DAW.

3. **Touch Screen Support**: While the plugin supports mouse input, touch screen support depends on JUCE's touch handling on your platform.

### Troubleshooting

**Plugin doesn't appear in DAW:**
- Verify the VST3 file is in the correct system folder
- Rescan plugins in your DAW
- Check DAW's plugin blacklist/whitelist settings
- Verify the plugin was built for the correct architecture (x64)

**No MIDI output:**
- Check MIDI routing in your DAW
- Verify an instrument is receiving MIDI from SquareBeats
- Check that squares are within the loop length
- Verify transport is playing

**Stuck notes:**
- Stop and restart playback
- This may indicate a bug - please report with reproduction steps

**UI doesn't appear or is corrupted:**
- Try resizing the plugin window
- Close and reopen the plugin
- Check graphics driver is up to date

**Preset doesn't load:**
- Verify the preset was saved with the same plugin version
- Check for DAW-specific preset compatibility issues
- Try creating a new preset to verify save/load works

### Reporting Issues

If you encounter issues during testing, please document:
1. DAW name and version
2. Operating system and version
3. Plugin version
4. Steps to reproduce the issue
5. Expected behavior vs actual behavior
6. Screenshots or screen recordings if applicable

## Performance Benchmarks

Expected performance characteristics:
- **CPU Usage**: < 5% at 120 BPM with 50 squares (on modern CPU)
- **UI Frame Rate**: 60 FPS during playback
- **Memory Usage**: < 50 MB
- **Latency**: Determined by DAW buffer size (plugin adds negligible latency)

## Development Testing

For developers working on the plugin, additional testing tools:

### Unit Tests

Build and run unit tests:
```bash
cd build
cmake --build . --config Debug
./PatternModelTests
./ConversionUtilsTests
./MIDIGeneratorTests
./PlaybackEngineTests
./StateManagerTests
```

### Debug Build

For debugging in a DAW:
1. Build Debug configuration
2. Install debug VST3 to system folder
3. Attach debugger to DAW process
4. Set breakpoints in plugin code

### MIDI Monitoring

Use a MIDI monitor tool to verify MIDI output:
- Windows: MIDI-OX, MIDIView
- macOS: MIDI Monitor
- Cross-platform: Protokol (by Hexler)

## Conclusion

This testing guide covers the essential functionality of SquareBeats. Thorough testing ensures the plugin works reliably across different DAWs and platforms. If all checklist items pass, the plugin is ready for release!

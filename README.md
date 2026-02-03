# SquareBeats VST3 Plugin

A VST3 MIDI sequencer plugin with a unique square-drawing interface built with JUCE.

## Project Structure

```
SquareBeats/
├── Source/
│   ├── DataStructures.h      # Core data structures (Square, ColorChannelConfig, etc.)
│   ├── PluginProcessor.h/cpp # Main audio processor
│   └── PluginEditor.h/cpp    # Plugin UI editor
├── CMakeLists.txt            # CMake build configuration
├── SquareBeats.jucer         # Projucer project file
└── README.md                 # This file
```

## Core Data Structures

### Square
Represents a MIDI note event on the sequencing plane with normalized coordinates (0.0 to 1.0):
- `leftEdge`: Horizontal position (time)
- `width`: Duration
- `topEdge`: Vertical position (pitch)
- `height`: Vertical size (velocity)
- `colorChannelId`: Assigned color channel (0-3)
- `uniqueId`: Unique identifier

### ColorChannelConfig
Configuration for each color channel:
- `midiChannel`: MIDI channel (1-16)
- `highNote`/`lowNote`: Pitch range (0-127)
- `quantize`: Quantization value (1/32 to 1 bar)
- `displayColor`: UI rendering color
- `pitchWaveform`: Per-color pitch sequencer waveform (semitones)
- `pitchSeqLoopLengthBars`: Per-color pitch sequencer loop length (1-64 bars)
- `getPitchOffsetAt()`: Get interpolated pitch offset at position

### ScaleConfig
Scale configuration for note filtering:
- `rootNote`: Root note (C through B)
- `scaleType`: Scale type (Chromatic, Major, Minor, modes, etc.)
- `snapToScale()`: Snap MIDI note to nearest scale degree

### PitchSequencer
Global pitch sequencer settings:
- `visible`: Show/hide state (waveforms and loop lengths are per-color in ColorChannelConfig)

### TimeSignature
Time signature configuration:
- `numerator`: 1-16
- `denominator`: 1, 2, 4, 8, 16
- `getBeatsPerBar()`: Calculate beats per bar

## Building the Project

### Prerequisites

1. **JUCE Framework**: Download from [juce.com](https://juce.com/get-juce)
   - Extract to a known location (e.g., `C:\JUCE` on Windows)
   - No installation required - just extract the archive
   
2. **CMake**: Version 3.15 or higher
   - Download from [cmake.org](https://cmake.org/download/)
   
3. **C++ Compiler with Build Tools**: 
   - **Windows**: Visual Studio 2022 (or 2019)
     - Install "Desktop development with C++" workload
     - Includes MSVC compiler and Windows SDK
   - **macOS**: Xcode 11 or later
     - Install Xcode Command Line Tools: `xcode-select --install`

### Quick Build (Recommended)

**Windows - Modern JUCE Approach:**
```cmd
# Run the build script (uses modern JUCE 7+ includes)
build_modern.bat
```

This script will:
1. Clean any previous build
2. Configure CMake with JUCE at `C:\JUCE`
3. Build the VST3 plugin in Release mode
4. Show the output location

**Manual Build with CMake:**

```bash
# Configure (JUCE_DIR should point to your JUCE installation)
cmake -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR="C:\JUCE"

# Build Release configuration
cmake --build build --config Release --target SquareBeats_VST3 -- /m:4
```

**macOS/Linux:**
```bash
# Configure
cmake -B build -DJUCE_DIR="/path/to/JUCE"

# Build
cmake --build build --config Release --target SquareBeats_VST3 -j4
```

### Build Output Location

After building, the VST3 plugin will be located at:
- **Windows**: `build\SquareBeats_artefacts\Release\VST3\SquareBeats.vst3`
- **macOS**: `build/SquareBeats_artefacts/Release/VST3/SquareBeats.vst3`

### Installation

Copy the entire `.vst3` folder (not just the file inside) to your DAW's VST3 plugin directory:
- **Windows**: `C:\Program Files\Common Files\VST3\`
- **macOS**: `~/Library/Audio/Plug-Ins/VST3/`

**Windows Installation Command:**
```cmd
xcopy /E /I /Y "build\SquareBeats_artefacts\Release\VST3\SquareBeats.vst3" "C:\Program Files\Common Files\VST3\SquareBeats.vst3"
```

### Modern JUCE Approach

This project uses **modern JUCE 7+ practices**:
- ✅ Individual module includes (e.g., `juce_audio_processors/juce_audio_processors.h`)
- ✅ No legacy `JuceHeader.h` file
- ✅ CMake-based build system
- ✅ Explicit module dependencies

This approach provides:
- Faster compilation times
- Better IDE code completion
- Clearer dependencies
- Future-proof codebase

### Troubleshooting Build Issues

**CMake can't find JUCE:**
```cmd
# Set JUCE_DIR explicitly
cmake -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR="C:\JUCE"
```

**Visual Studio not found:**
- Install Visual Studio 2022 with "Desktop development with C++" workload
- Or use Visual Studio 2019: `-G "Visual Studio 16 2019"`

**Build fails with missing modules:**
- Ensure JUCE is fully extracted (not just the root folder)
- Check that `C:\JUCE\CMakeLists.txt` exists

**Clean build:**
```cmd
rmdir /s /q build
build_modern.bat
```

## Development Status

**Current Version:** 1.0.0 (Release Candidate)

Implementation progress:
- ✅ JUCE project structure and core data structures (Task 1)
- ✅ Pattern model with square management (Task 2)
- ✅ Coordinate conversion utilities (Task 3)
- ✅ MIDI generator with quantization (Task 4)
- ✅ Playback engine with tempo sync (Task 6)
- ✅ Pitch sequencer functionality (Task 7)
- ✅ State manager for serialization (Task 9)
- ✅ VST3 audio processor integration (Task 10)
- ✅ UI components - Sequencing plane (Task 11)
- ✅ UI components - Controls and configuration (Task 12)
- ✅ Pitch sequencer UI (Task 13)
- ✅ Main editor layout (Task 14)
- ✅ Error handling and edge cases (Task 16)
- ✅ Loop length change handling (Task 17)
- ✅ Final integration and polish (Task 18)

**Ready for DAW testing!**

## Quick Start

### 1. Building the Plugin

**Windows (Recommended):**
```cmd
# Make sure JUCE is installed at C:\JUCE
build_modern.bat
```

**Manual Build:**
```cmd
# Configure CMake
cmake -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR="C:\JUCE"

# Build
cmake --build build --config Release --target SquareBeats_VST3 -- /m:4
```

**macOS/Linux:**
```bash
# Configure
cmake -B build -DJUCE_DIR="/path/to/JUCE"

# Build
cmake --build build --config Release --target SquareBeats_VST3 -j4
```

### 2. Installing the Plugin

**Windows:**
```cmd
xcopy /E /I /Y "build\SquareBeats_artefacts\Release\VST3\SquareBeats.vst3" "C:\Program Files\Common Files\VST3\SquareBeats.vst3"
```

**macOS:**
```bash
cp -r "build/SquareBeats_artefacts/Release/VST3/SquareBeats.vst3" "~/Library/Audio/Plug-Ins/VST3/"
```

### 3. Testing the Plugin

1. **Rescan Plugins**: Open your DAW and rescan VST3 plugins
2. **Create MIDI Track**: Add SquareBeats as a MIDI effect
3. **Add Instrument**: Route MIDI output to a software instrument
4. **Draw Squares**: Click and drag on the sequencing plane to create notes
5. **Delete Squares**: Double-click any square to remove it
6. **Configure Colors**: Select different colors for different MIDI channels
7. **Pitch Sequencer**: Click "Show Pitch Sequencer" and draw pitch modulation curves per color
8. **Pitch Seq Length**: Use the "Pitch Len:" dropdown to set each color's pitch sequencer loop length (1-64 bars)
9. **Scale Selection**: Choose root note and scale type to constrain notes to a musical scale
10. **Start Playback**: Press play in your DAW to hear your pattern

For detailed testing instructions, see [BUILD_AND_TEST.md](BUILD_AND_TEST.md)

## Documentation

- **[BUILD_AND_TEST.md](BUILD_AND_TEST.md)** - Comprehensive build and testing guide
- **[TESTING_RESULTS.md](TESTING_RESULTS.md)** - Testing checklist and results template
- **[.kiro/specs/squarebeats-vst3/](../.kiro/specs/squarebeats-vst3/)** - Complete specification documents
  - `requirements.md` - Feature requirements
  - `design.md` - Architecture and design
  - `tasks.md` - Implementation task list

## Features

### Core Functionality
- **Visual MIDI Sequencing**: Draw squares on a 2D plane to create MIDI patterns
- **4 Color Channels**: Independent MIDI routing and configuration per color
- **Tempo Synchronization**: Tight integration with host DAW tempo and transport
- **Flexible Quantization**: Per-color quantization from 1/32 note to 1 bar
- **Scale Filtering**: Snap notes to musical scales (16 scales including Major, Minor, modes, Pentatonic, Blues, etc.)
- **Per-Color Pitch Sequencer**: Each color has its own pitch modulation waveform and independent loop length (1-64 bars for polyrhythmic patterns)
- **Loop Lengths**: 1, 2, or 4 bar patterns (controls playback speed, not square positions)
- **Time Signatures**: Support for various time signatures (1-16 numerator, 1/2/4/8/16 denominator)
- **Preset Support**: Full state saving/loading through DAW preset system

### User Interface
- **Resizable Window**: 800x600 minimum, 2000x1500 maximum
- **Real-time Playback Indicators**: Visual feedback for both main sequencer and pitch sequencer positions
- **Color Configuration Panel**: Per-color settings for quantization, pitch range, and MIDI channel
- **Loop Length Selector**: Quick switching between 1, 2, and 4 bar loops
- **Scale Controls**: Root note and scale type selection (Chromatic, Major, Minor, Harmonic Minor, Melodic Minor, Pentatonic Major/Minor, Blues, Dorian, Phrygian, Lydian, Mixolydian, Locrian, Whole Tone, Diminished)
- **Pitch Sequencer Length**: Per-color dropdown for 1-64 bar pitch sequencer loops
- **Time Signature Controls**: Easy time signature configuration
- **Clear All Button**: Remove all squares and pitch waveforms from all colors (top bar)
- **Clear Color Button**: Remove all squares of selected color (side panel)
- **Clear Pitch Sequencer Button**: Reset the selected color's pitch waveform
- **Double-Click to Delete**: Double-click any square to remove it

### Technical Features
- **Monophonic Voice Management**: One note per color channel at a time
- **Cross-Platform**: Windows and macOS support
- **VST3 Standard**: Full VST3 compliance with state serialization
- **Normalized Coordinates**: Resolution-independent square positioning
- **Error Handling**: Robust handling of edge cases and invalid input

## Requirements Covered

All requirements from the specification are implemented:
- ✅ Requirement 1: Square drawing and editing
- ✅ Requirement 2: Color channel management
- ✅ Requirement 3: MIDI event generation
- ✅ Requirement 4: Pitch sequencer layer
- ✅ Requirement 5: Tempo synchronization and playback
- ✅ Requirement 6: Loop length and time signature configuration
- ✅ Requirement 7: State persistence
- ✅ Requirement 8: User interface layout
- ✅ Requirement 9: Cross-platform compatibility
- ✅ Requirement 10: MIDI note mapping

## Known Limitations

1. **Property-Based Tests**: Optional PBT tasks were skipped for faster MVP delivery
2. **Touch Screen**: Touch input support depends on JUCE's platform implementation
3. **Preset Format**: Presets are DAW-specific (not portable between different DAWs)

## Ableton Live Compatibility Notes

**Critical CMake Settings for Ableton Live:**

The following settings in `CMakeLists.txt` are required for the plugin to load in Ableton Live:

```cmake
juce_add_plugin(SquareBeats
    ...
    IS_SYNTH TRUE              # Must be TRUE - Ableton doesn't support pure MIDI effects
    IS_MIDI_EFFECT FALSE       # Must be FALSE for Ableton compatibility
    NEEDS_MIDI_INPUT TRUE      # CRITICAL: Must be TRUE or Ableton will fail to load the plugin
    NEEDS_MIDI_OUTPUT TRUE     # Required for MIDI generation
    VST3_CATEGORIES "Instrument|Generator"
)
```

**Why these settings matter:**
- `NEEDS_MIDI_INPUT TRUE` is essential - without this, Ableton Live will show "The VST3 plug-in could not be opened" error
- `IS_SYNTH TRUE` with `IS_MIDI_EFFECT FALSE` ensures the plugin appears under Instruments in Ableton
- The plugin must have audio output buses configured (even if outputting silence) for Ableton to accept it

**Using SquareBeats in Ableton Live:**
1. Load SquareBeats on a MIDI track (it appears under Instruments)
2. Create another MIDI track with a synth instrument
3. On the SquareBeats track, set "MIDI To" to route to the synth track
4. Draw squares and press play to hear your pattern

## Contributing

This project follows the spec-driven development methodology. See the task list in `.kiro/specs/squarebeats-vst3/tasks.md` for implementation details.

## License

[Add your license information here]

## Credits

Built with [JUCE](https://juce.com/) framework for cross-platform audio plugin development.

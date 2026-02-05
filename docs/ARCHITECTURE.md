# SquareBeats Architecture

## Overview

SquareBeats is a VST3 MIDI sequencer plugin built with the JUCE framework. It follows a clean separation between data model, audio processing, and user interface.

## Core Components

### Data Structures (`DataStructures.h`)

Central data types used throughout the plugin:

#### Square
Represents a MIDI note event with normalized coordinates (0.0 to 1.0):
- `leftEdge`, `width`: Horizontal position and duration (time)
- `topEdge`, `height`: Vertical position and size (pitch/velocity)
- `colorChannelId`: Assigned color channel (0-3)
- `uniqueId`: Unique identifier for tracking

#### ColorChannelConfig
Configuration for each of the 4 color channels:
- `midiChannel`: Output MIDI channel (1-16)
- `highNote`, `lowNote`: Pitch range (0-127)
- `quantize`: Quantization value (1/32 to 1 bar)
- `displayColor`: UI rendering color
- `pitchWaveform`: Per-color pitch sequencer waveform
- `pitchSeqLoopLengthBars`: Per-color pitch sequencer loop (0 = use global, 1-64 = bars)
- `mainLoopLengthBars`: Per-color main loop override (0 = use global)
- `getPitchOffsetAt()`: Get interpolated pitch offset at position

#### ScaleConfig
Musical scale configuration:
- `rootNote`: Root note (C through B)
- `scaleType`: Scale type (Chromatic, Major, Minor, modes, etc.)
- `snapToScale()`: Snap MIDI note to nearest scale degree

#### ScaleSequencerConfig
Scale sequencer for harmonic progressions:
- `enabled`: Whether scale sequencing is active
- `segments`: Vector of ScaleSequenceSegment (max 16)
- `getTotalLengthBars()`: Get total sequence length
- `getScaleAtPosition()`: Get active scale at bar position

#### PlayModeConfig
Playback direction and probability:
- `mode`: Forward, Backward, Pendulum, or Probability
- `stepJumpSize`: Normalized step jump size (0.0-1.0 → 1, 2, 4, 8, 16 steps)
- `probability`: Chance of jumping vs normal step (0.0-1.0)
- `pendulumForward`: Internal state for pendulum direction

#### TimeSignature
Time signature configuration:
- `numerator`: 1-16
- `denominator`: 1, 2, 4, 8, 16
- `getBeatsPerBar()`: Calculate beats per bar

### Pattern Model (`PatternModel.h/cpp`)

Manages the collection of squares and provides operations:
- Add, remove, find squares
- Get squares by color channel
- Loop length management (preserves squares on change)
- Color channel configuration
- Scale configuration
- Scale sequencer configuration
- Play mode configuration
- Time signature

**Key Methods:**
- `addSquare()`, `removeSquare()`, `findSquareAt()`
- `getSquaresByColor()`: Filter squares by color channel
- `setLoopLengthBars()`: Change loop length (handles overflow)
- `getColorConfig()`, `setColorConfig()`: Per-color settings
- `getScale()`, `setScale()`: Musical scale settings
- `getScaleSequencer()`: Scale sequencer configuration
- `getPlayMode()`, `setPlayMode()`: Playback mode settings

### MIDI Generator (`MIDIGenerator.h/cpp`)

Converts squares to MIDI events:
- `calculateMidiNote()`: Map vertical position to MIDI note (respects pitch range and scale)
- `calculateVelocity()`: Map square height to velocity (1-127)
- `applyQuantization()`: Snap timing to grid
- `generateMidiForSquare()`: Create MIDI events for a square

**Quantization Support:**
- 1/32, 1/16, 1/8, 1/4, 1/2, 1 bar
- Snaps note-on and note-off independently

### Playback Engine (`PlaybackEngine.h/cpp`)

Handles tempo-synchronized playback and MIDI generation:
- Transport synchronization with DAW
- Per-color position tracking for independent loop lengths
- Per-color pendulum direction tracking
- Per-color step tracking for probability mode
- MIDI event generation and buffering
- Monophonic voice management per color

**Key Methods:**
- `processBlock()`: Main audio callback, generates MIDI events
- `updatePlaybackPosition()`: Advance playback based on tempo and play mode
- `processColorTriggers()`: Process squares for a specific color
- `getNormalizedPlaybackPositionForColor()`: Get per-color playback position
- `resetPlaybackPosition()`: Reset on transport stop

**Play Mode Implementation:**
- **Forward**: Linear advance with wrap-around
- **Backward**: Reverse advance with wrap-around
- **Pendulum**: Bounce at loop boundaries (per-color direction)
- **Probability**: Step-based with random forward jumps (per-color steps)

### State Manager (`StateManager.h/cpp`)

Serializes/deserializes plugin state for presets:
- Binary format with magic number and version
- Saves all squares, configurations, and settings
- Validates data on load
- Backward compatible with older versions

**Binary Format:**
```
[Magic: 4 bytes] [Version: 4 bytes]
[Loop Length] [Time Signature]
[Scale Config] [Scale Sequencer Config]
[Play Mode Config]
[Color Configs × 4]
[Square Count] [Squares...]
```

### Preset Manager (`PresetManager.h/cpp`)

Manages preset files on disk:
- Standard VST3 preset locations
- `.vstpreset` file format
- Save, load, delete, list presets
- Factory preset creation

**Preset Locations:**
- Windows: `Documents/VST3 Presets/Touchmachines/SquareBeats/`
- macOS: `/Library/Audio/Presets/Touchmachines/SquareBeats/`
- Linux: `~/.vst3/presets/Touchmachines/SquareBeats/`

### Conversion Utils (`ConversionUtils.h`)

Utility functions for coordinate conversion:
- `normalizedToBeats()`: Convert normalized time to beats
- `beatsToNormalized()`: Convert beats to normalized time
- `mapVerticalPositionToNote()`: Map Y position to MIDI note
- `mapHeightToVelocity()`: Map square height to velocity

## UI Components

### Plugin Editor (`PluginEditor.h/cpp`)

Main editor window:
- Layout management (sequencing plane + right panel)
- Component coordination
- Timer for playback position updates
- Preset UI (dropdown, save/delete buttons)
- Logo click handler for Help/About dialog

### Sequencing Plane (`SequencingPlaneComponent.h/cpp`)

Main drawing area:
- Square drawing, moving, resizing, deleting
- Visual feedback (gate flashes, velocity ripples, active square glow)
- Playback indicators (per-color playheads with motion trails)
- Beat pulse grid
- Mouse interaction handling

### Color Selector (`ColorSelectorComponent.h/cpp`)

4-color channel selector:
- Visual color squares
- Active channel highlighting
- Activity LEDs (show which channels are triggering)

### Color Config Panel (`ColorConfigPanel.h/cpp`)

Per-color configuration:
- SQUARES/PITCH tabs with custom look and feel
- Quantization dropdown
- High/Low MIDI note sliders
- MIDI channel selector
- Pitch sequencer loop length dropdown
- Main loop length dropdown (per-color override)
- Clear button (context-sensitive)

### Pitch Sequencer Component (`PitchSequencerComponent.h/cpp`)

Pitch modulation editor:
- Waveform drawing and editing
- Per-color waveforms
- Playback position indicator with glow
- Independent loop length per color

### Scale Controls (`ScaleControls.h/cpp`)

Musical scale selection:
- Root note dropdown
- Scale type dropdown
- Disabled when scale sequencer is active
- Shows currently playing scale in real-time

### Scale Sequencer Component (`ScaleSequencerComponent.h/cpp`)

Timeline editor for scale sequences:
- Segment visualization
- Click to edit segment (root/scale/duration)
- Drag edges to resize segments
- Add/remove segments
- Playback position indicator
- Scale change flash effect

### Play Mode Controls (`PlayModeControls.h/cpp`)

Playback direction buttons:
- Forward, Backward, Pendulum, Probability
- Visual indication of active mode
- XY pad for probability mode (appears in color config panel)

### Loop Length Selector (`LoopLengthSelector.h/cpp`)

Global loop length dropdown:
- 1-15 steps, 1-8 bars, 16/32/64 bars
- Updates pattern model on change

### Help/About Dialog (`HelpAboutDialog.h/cpp`)

Modal dialog with plugin information:
- Quick start guide
- Feature overview
- Website link
- Copyright information

## Audio Processor (`PluginProcessor.h/cpp`)

VST3 audio processor implementation:
- Owns PatternModel, PlaybackEngine, PresetManager
- Implements VST3 callbacks
- State save/load integration
- MIDI output configuration
- Parameter management

**VST3 Configuration:**
- `IS_SYNTH TRUE`: Required for Ableton Live compatibility
- `IS_MIDI_EFFECT FALSE`: Appears as instrument
- `NEEDS_MIDI_INPUT TRUE`: Critical for Ableton Live
- `NEEDS_MIDI_OUTPUT TRUE`: Generates MIDI

## Threading Model

### Audio Thread
- `processBlock()`: Generates MIDI events
- Lock-free communication with UI thread
- No allocations in audio callback

### UI Thread
- Timer-based updates (60 FPS)
- Reads playback position from audio thread
- Handles user interaction
- Updates visual feedback

### Thread Safety
- Atomic variables for playback position
- Lock-free FIFO for visual feedback events
- No shared mutable state between threads

## Build System

### CMake (`CMakeLists.txt`)
- Modern JUCE 7+ approach
- Individual module includes (no JuceHeader.h)
- VST3 target configuration
- Test executable targets

### Projucer (`SquareBeats.jucer`)
- Alternative build method
- IDE project generation
- Module configuration

## Dependencies

### JUCE Framework
Required modules:
- `juce_audio_processors`: VST3 plugin framework
- `juce_audio_utils`: Audio utilities
- `juce_gui_basics`: UI components
- `juce_gui_extra`: Advanced UI features
- `juce_core`: Core utilities
- `juce_data_structures`: Data structures
- `juce_events`: Event handling
- `juce_graphics`: Graphics rendering

### System Fonts
- Windows: Segoe UI
- macOS: SF Pro Text
- Linux: Ubuntu

## Design Patterns

### Model-View-Controller
- **Model**: PatternModel (data)
- **View**: UI Components (presentation)
- **Controller**: PluginEditor, PlaybackEngine (logic)

### Observer Pattern
- UI components observe model changes
- Timer-based polling for playback position

### Strategy Pattern
- Play modes (Forward, Backward, Pendulum, Probability)
- Quantization strategies

### Factory Pattern
- Preset creation (factory presets)

## Error Handling

### Validation
- Input validation at boundaries
- Coordinate clamping (0.0-1.0)
- MIDI note clamping (0-127)
- Loop length validation

### Logging
- JUCE Logger for diagnostics
- Error messages for user feedback
- Debug assertions in development builds

### Graceful Degradation
- Invalid presets rejected with error message
- Corrupted state data handled safely
- Missing files handled gracefully

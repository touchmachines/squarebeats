# SquareBeats Features

## Core Functionality

### Visual MIDI Sequencing
Draw squares on a 2D plane to create MIDI patterns. Each square represents a MIDI note with:
- **Horizontal position**: Note timing (when it plays)
- **Horizontal width**: Note duration (how long it plays)
- **Vertical position**: Pitch (which note)
- **Vertical height**: Velocity (how loud)

### 4 Color Channels
Independent MIDI routing and configuration per color:
- Each color can output to a different MIDI channel (1-16)
- Per-color pitch range (high/low MIDI notes 0-127)
- Per-color quantization (1/32 note to 1 bar)
- Per-color loop length (1-64 bars) for polyrhythmic patterns
- Per-color pitch sequencer with independent loop length

### Tempo Synchronization
Tight integration with host DAW:
- Automatically syncs to DAW tempo (BPM)
- Responds to transport start/stop
- Handles tempo changes in real-time
- Defaults to 120 BPM if host tempo unavailable

### Musical Scales
Snap notes to musical scales with 16 scale types:
- Chromatic (all notes)
- Major, Minor, Harmonic Minor, Melodic Minor
- Pentatonic Major, Pentatonic Minor
- Blues
- Dorian, Phrygian, Lydian, Mixolydian, Locrian
- Whole Tone, Diminished

### Scale Sequencer
Chain multiple key/scale changes with independent timing:
- Up to 16 segments per sequence
- Each segment: 1-16 bars duration
- Choose root note and scale type per segment
- Creates evolving harmonic progressions
- Cycles independently of main pattern
- Visual timeline editor with drag-to-resize

### Per-Color Pitch Sequencer
Each color has its own pitch modulation:
- Draw pitch curves that add semitone offsets (-12 to +12)
- Independent loop length per color (1-64 bars)
- Creates polyrhythmic pitch patterns
- Always active (modulation applies regardless of editing mode)
- Smooth interpolation between waveform points

### Play Modes
Four playback direction modes:
- **Forward (-->)**: Normal sequential playback
- **Backward (<--)**: Reverse playback
- **Pendulum (<-->)**: Bounces back and forth at loop boundaries
- **Probability (--?>)**: Random forward jumps with configurable:
  - Step jump size (1, 2, 4, 8, or 16 steps) - musically aligned
  - Probability (0-100% chance of jumping vs normal step)
  - XY pad control in side panel

### Loop Lengths
Flexible loop length options:
- 1-15 steps (16th note grid)
- 1-8 bars
- 16, 32, 64 bars
- Controls playback speed, not square positions
- Per-color override available for polyrhythms

### Preset System
Full state saving/loading:
- Save/load patterns and all settings
- Standard VST3 preset locations
- Compatible with DAW preset browsers
- Factory "_Init" preset for quick reset
- Presets include:
  - All squares (position, size, color)
  - Color configurations
  - Pitch sequencer waveforms
  - Scale settings
  - Scale sequencer configuration
  - Play mode settings
  - Loop lengths

## User Interface

### Resizable Window
- Minimum: 800x600 pixels
- Maximum: 2000x1500 pixels
- Proportional scaling of all UI elements

### Layout
- **Left side**: Full-height sequencing plane for drawing
- **Right panel (280px)**: All controls organized vertically:
  1. Play mode buttons (top)
  2. Global controls (loop length, clear all)
  3. Color selector and configuration
  4. Scale controls
  5. Logo (bottom, clickable for Help/About)

### Tab-Based Color Configuration
- **SQUARES tab**: Edit squares, configure quantization/pitch/MIDI
- **PITCH tab**: Edit pitch sequencer waveform
- Clear button is context-sensitive (clears squares or pitch waveform)
- Pitch sequencer length dropdown always visible

### Visual Feedback
Dynamic visual feedback makes the plugin fun to watch:
- **Gate Flash Background**: Color washes flash when notes trigger
- **Velocity Ripples**: Expanding rings from active squares, scaled by velocity
- **Active Square Glow**: Currently playing square pulses
- **Playhead Motion Trail**: Gradient trail shows direction
- **Beat Pulse Grid**: Grid brightens on beats (stronger on downbeats)
- **Color Activity LEDs**: Show which channels are triggering
- **Pitch Sequencer Glow**: Glowing dot tracks pitch offset
- **Scale Change Flash**: Highlights scale sequencer transitions

### Interaction
- **Click and drag**: Create new square
- **Double-click**: Delete square
- **Drag square**: Move position
- **Drag edge**: Resize square
- **Color selector**: Switch between 4 color channels
- **XY pad**: Control probability mode (appears only when active)

## Technical Features

### Monophonic Voice Management
- One note per color channel at a time
- Automatic note-off before new note-on
- Different colors play independently (polyphonic across colors)
- No stuck notes

### Cross-Platform
- Windows and macOS support
- Standard VST3 format
- Platform-specific system fonts (Segoe UI, SF Pro Text, Ubuntu)

### Performance
- < 5% CPU usage at 120 BPM with 50 squares
- 60 FPS UI animation
- < 50 MB memory usage
- Thread-safe visual feedback (lock-free audio/UI communication)

### Error Handling
- Robust handling of edge cases
- Validates all user input
- Graceful degradation on errors
- Comprehensive logging

### State Persistence
- Binary serialization format
- Version checking for compatibility
- Cross-platform preset sharing
- Backward compatible state loading

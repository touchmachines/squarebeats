# SquareBeats User Guide

## Getting Started

### Installation

1. **Build the plugin** (see BUILD_AND_TEST.md)
2. **Copy to VST3 folder:**
   - Windows: `C:\Program Files\Common Files\VST3\SquareBeats.vst3`
   - macOS: `~/Library/Audio/Plug-Ins/VST3/SquareBeats.vst3`
3. **Rescan plugins** in your DAW
4. **Load SquareBeats** on a MIDI track
5. **Route MIDI** to a software instrument

### Quick Start

1. **Draw squares** - Click and drag on the sequencing plane
2. **Delete squares** - Double-click any square
3. **Select colors** - Click one of the 4 color squares in the right panel
4. **Configure colors** - Use the SQUARES tab to set quantization, pitch range, MIDI channel
5. **Edit pitch** - Click the PITCH tab to draw pitch modulation curves
6. **Choose scale** - Select root note and scale type
7. **Set play mode** - Choose Forward, Backward, Pendulum, or Probability
8. **Press play** - Start your DAW to hear the pattern

## Interface Overview

### Sequencing Plane (Left Side)
The main drawing area where you create your pattern:
- **Horizontal axis**: Time (left to right)
- **Vertical axis**: Pitch (bottom to top)
- **Square size**: Duration (width) and velocity (height)

### Right Panel (280px)
All controls organized top to bottom:

1. **Play Mode Buttons**
   - Forward (-->), Backward (<--), Pendulum (<-->), Probability (--?>)

2. **Global Controls**
   - Loop Length dropdown
   - Clear All button

3. **Color Selector**
   - 4 color squares
   - Activity LEDs show which channels are playing

4. **Color Configuration**
   - SQUARES/PITCH tabs
   - Per-color settings
   - XY pad (Probability mode only)

5. **Scale Controls**
   - Root note dropdown
   - Scale type dropdown
   - Scale Seq toggle button

6. **Logo (Bottom)**
   - Click for Help/About dialog

## Drawing Squares

### Creating Squares
1. Click and drag on the sequencing plane
2. Release to create the square
3. Square appears in the selected color

### Moving Squares
1. Click and drag an existing square
2. Move to new position
3. Release to place

### Resizing Squares
1. Click and drag a square edge
2. Resize horizontally (duration) or vertically (velocity)
3. Release when done

### Deleting Squares
- Double-click any square to remove it
- Or use "Clear All" to remove all squares

## Color Channels

### Selecting Colors
Click one of the 4 color squares in the right panel to:
- Draw new squares in that color
- Edit settings for that color
- View/edit that color's pitch sequencer

### Configuring Colors (SQUARES Tab)

**Quantization:**
- Snap note timing to grid
- Options: 1/32, 1/16, 1/8, 1/4, 1/2, 1 bar

**High/Low MIDI:**
- Set pitch range for this color
- High: Highest note (top of plane)
- Low: Lowest note (bottom of plane)

**MIDI Channel:**
- Output MIDI channel (1-16)
- Route to different instruments

**Pitch Len:**
- Pitch sequencer loop length (1-64 bars)
- Independent from main loop

**Loop Len:**
- Main loop length override for this color
- "Global" = use global loop length
- 1-64 bars = independent loop length
- Creates polyrhythmic patterns

### Per-Color Pitch Sequencer

Each color has its own pitch modulation:

1. Click the **PITCH tab**
2. Draw a pitch curve by clicking and dragging
3. Curve adds semitone offset (-12 to +12) to all notes
4. Set independent loop length with "Pitch Len:" dropdown
5. Pitch modulation is always active

**Use Cases:**
- Add vibrato or pitch bends
- Create polyrhythmic pitch patterns
- Different pitch modulation per color

## Musical Scales

### Selecting Scales
1. Choose **Root Note** (C through B)
2. Choose **Scale Type**:
   - Chromatic (all notes)
   - Major, Minor, Harmonic Minor, Melodic Minor
   - Pentatonic Major, Pentatonic Minor
   - Blues
   - Dorian, Phrygian, Lydian, Mixolydian, Locrian
   - Whole Tone, Diminished

All generated MIDI notes snap to the selected scale.

### Scale Sequencer

Create evolving harmonic progressions:

1. Click **"Scale Seq"** to enable
2. Root/Scale dropdowns become disabled
3. Timeline editor appears at bottom
4. Click **"+"** to add segments
5. Click a segment to edit:
   - Root note
   - Scale type
   - Duration (1-16 bars)
6. Drag segment edges to resize
7. Click **"Scale Seq"** again to disable

**Features:**
- Up to 16 segments
- Each segment: 1-16 bars
- Cycles independently of main pattern
- Real-time display of current scale

## Play Modes

### Forward (-->)
Normal sequential playback from left to right.

### Backward (<--)
Reverse playback from right to left.

### Pendulum (<-->)
Bounces back and forth at loop boundaries.
- Each color bounces at its own loop boundary
- Creates interesting polyrhythmic patterns

### Probability (--?>)
Random forward jumps with musical timing:

**XY Pad Controls** (appears in color config panel):
- **X-axis (Step Jump Size)**: How far to jump
  - Left: 1 step (subtle variation)
  - Center: 4 steps (quarter-bar)
  - Right: 16 steps (full bar)
  - Values: 1, 2, 4, 8, or 16 steps (musically aligned)

- **Y-axis (Probability)**: How often to jump
  - Bottom: 0% (never jump, normal playback)
  - Center: 50% (jump half the time)
  - Top: 100% (always jump)

**Behavior:**
- Advances normally on step boundaries
- Rolls for probability jump at each step
- Jumps are always forward
- Each color jumps independently

## Loop Lengths

### Global Loop Length
Set in the top right panel:
- 1-15 steps (16th note grid)
- 1-8 bars
- 16, 32, 64 bars

### Per-Color Loop Length
Set in the color config panel (SQUARES tab):
- "Global" = use global loop length
- 1-64 bars = independent loop length
- Creates polyrhythmic patterns

**Example:**
- Red: 2 bars (fast)
- Blue: 4 bars (medium)
- Green: 3 bars (creates 4:3 polyrhythm)
- Yellow: Global (follows global setting)

## Presets

### Saving Presets
1. Create your pattern
2. Click **"Save"** button
3. Enter preset name
4. Click **"Save"** in dialog
5. Preset appears in dropdown

### Loading Presets
1. Click preset dropdown
2. Select a preset
3. Pattern loads instantly

### Deleting Presets
1. Select preset from dropdown
2. Click **"Delete"** button
3. Confirm deletion

### Factory Presets
**_Init**: Resets to default settings
- Empty pattern
- 1 bar loop
- C Major scale
- Forward play mode
- Default color configurations

### Sharing Presets
Preset files are stored in:
- Windows: `Documents/VST3 Presets/Touchmachines/SquareBeats/`
- macOS: `/Library/Audio/Presets/Touchmachines/SquareBeats/`

Copy `.vstpreset` files to share with others.

## Visual Feedback

### Gate Flash Background
Semi-transparent color washes flash when notes trigger.
- Multiple colors blend additively
- Smooth exponential decay

### Velocity Ripples
Expanding rings from active squares.
- Size and intensity scale with velocity
- Harder hits = bigger ripples

### Active Square Glow
Currently playing square pulses with glow effect.
- Easy to track which square is active

### Playhead Motion Trail
Gradient trail behind playback indicator.
- Shows direction and speed
- One playhead per color (when using per-color loop lengths)

### Beat Pulse Grid
Grid lines brighten on beats.
- Stronger pulse on downbeats
- Interface "breathes" with the music

### Color Activity LEDs
Small LEDs in color selector.
- Show which channels are triggering notes

### Pitch Sequencer Glow
Glowing dot tracks pitch offset value.
- Shows current pitch modulation

### Scale Change Flash
Brief flash when scale sequencer changes segments.
- Highlights harmonic transitions

## Tips and Tricks

### Creating Grooves
- Use quantization to lock to grid
- Vary square heights for velocity dynamics
- Layer different colors for complexity

### Polyrhythms
- Set different loop lengths per color
- Try 2:3, 3:4, or 4:5 ratios
- Combine with different pitch sequencer lengths

### Evolving Patterns
- Use scale sequencer for harmonic movement
- Use probability mode for variation
- Combine pendulum mode with per-color loops

### Pitch Modulation
- Draw smooth curves for vibrato
- Draw stepped patterns for arpeggios
- Use different pitch lengths per color for polyrhythmic pitch

### Performance
- Save variations as presets
- Use Clear All for quick reset
- Use _Init preset to start fresh

## Troubleshooting

### No Sound
- Check MIDI routing to instrument
- Verify squares are within loop length
- Check DAW transport is playing
- Verify MIDI channel matches instrument

### Stuck Notes
- Stop and restart playback
- Check for overlapping squares (monophonic per color)

### UI Issues
- Try resizing window
- Close and reopen plugin
- Update graphics drivers

### Preset Issues
- Verify preset file exists
- Check file permissions
- Try creating new preset to test save/load

## Keyboard Shortcuts

- **Delete key**: Delete selected square (if implemented)
- **ESC**: Close Help/About dialog

## DAW-Specific Notes

### Ableton Live
- Load SquareBeats on MIDI track
- Set "MIDI To" to route to instrument track
- Plugin appears under Instruments

### FL Studio
- Load as MIDI Out generator
- Route to instrument channel

### Reaper
- Load as MIDI effect
- Route MIDI to instrument track

### Other DAWs
- Load on MIDI track or as MIDI effect
- Route MIDI output to instrument
- Consult DAW documentation for MIDI routing

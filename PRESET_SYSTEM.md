# SquareBeats Preset System

## Overview

SquareBeats includes a comprehensive preset management system that allows users to save, load, and organize their patterns and settings.

## Features

- **Save Presets**: Save the complete state of the plugin including:
  - All squares (position, size, color assignments)
  - Color channel configurations (MIDI channel, pitch range, quantization)
  - Pitch sequencer waveforms and loop lengths
  - Scale settings (root note, scale type)
  - Scale sequencer configuration
  - Play mode settings
  - Loop length
  
- **Load Presets**: Instantly recall saved patterns
- **Delete Presets**: Remove unwanted presets with confirmation
- **Factory Presets**: Ships with a "_Init" preset for quick reset (underscore prefix keeps it at the top of the list)

## UI Controls

Located in the top right panel:

```
[Preset Dropdown ▼] [Save] [Delete]
```

- **Preset Dropdown**: Select from available presets
- **Save Button**: Save current state as a new preset
- **Delete Button**: Remove the selected preset

## Preset Storage

Presets are stored as `.vstpreset` files in the standard VST3 preset location:

- **Windows**: `C:\Users\[Username]\Documents\VST3 Presets\Touchmachines\SquareBeats\`
- **macOS**: `/Library/Audio/Presets/Touchmachines/SquareBeats/`
- **Linux**: `~/.vst3/presets/Touchmachines/SquareBeats/`

This standard location means:
- DAWs can automatically discover and display your presets
- Presets are easy to backup and share
- Presets work across different DAWs

## Usage

### Saving a Preset

1. Create your pattern and configure settings
2. Click the **Save** button
3. Enter a name for your preset
4. Click **Save** in the dialog
5. If a preset with that name exists, you'll be asked to confirm overwrite

### Loading a Preset

1. Click the **Preset Dropdown**
2. Select a preset from the list
3. The plugin state will immediately update

### Deleting a Preset

1. Select a preset from the dropdown
2. Click the **Delete** button
3. Confirm the deletion

### Factory Presets

**_Init**: Resets all settings to defaults (underscore prefix keeps it at the top of the list)
- 1 bar loop length
- 4/4 time signature
- No squares
- Default color configurations
- C Major scale
- Forward play mode
- All pitch sequencers at 1 bar length

## Technical Details

### File Format

Presets use the same binary serialization format as the plugin's state save/load system (`StateManager`). The format includes:

- Magic number for validation
- Version number for compatibility
- All pattern data
- All configuration settings

### Compatibility

- Presets are forward and backward compatible within the same major version
- The system handles version differences gracefully
- Invalid or corrupted presets are rejected with error messages

## Implementation

The preset system consists of:

- **PresetManager** (`Source/PresetManager.h/cpp`): Core preset file management
- **PluginProcessor**: Integration with audio processor
- **PluginEditor**: UI controls and user interaction

## Sharing Presets

To share presets with other users:

1. Navigate to the preset directory (see Preset Storage above)
2. Copy the `.vstpreset` files you want to share
3. Recipients place the files in their preset directory
4. Presets appear automatically in the dropdown

## Troubleshooting

**Presets not appearing:**
- Check that the preset directory exists
- Verify files have `.vstpreset` extension
- Restart the plugin or DAW

**Save fails:**
- Check write permissions for the preset directory
- Ensure disk space is available
- Check the log for error messages

**Load fails:**
- Preset file may be corrupted
- Preset may be from an incompatible version
- Check the log for detailed error information

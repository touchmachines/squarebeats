# Help/About Dialog Feature

## Overview

Added a clickable Help/About dialog that appears when users click on the SquareBeats logo in the top-left corner of the plugin interface.

## Implementation

### New Files Created

1. **HelpAboutDialog.h** - Header file defining the dialog component
2. **HelpAboutDialog.cpp** - Implementation with content from README.md

### Modified Files

1. **PluginEditor.h** - Added mouseDown handler and logo click area tracking
2. **PluginEditor.cpp** - Implemented logo click detection and dialog launching
3. **CMakeLists.txt** - Added new source files to build configuration

## Features

The Help/About dialog displays:

- **Plugin Title & Version**: SquareBeats Version 1.0.0
- **Description**: Brief overview of the plugin
- **Quick Start Guide**: 9-step getting started instructions
  1. Draw squares on the sequencing plane
  2. Double-click to delete squares
  3. Select colors for different MIDI channels
  4. Switch between SQUARES/PITCH editing modes
  5. Configure quantization and pitch ranges
  6. Choose musical scales
  7. Enable Scale Sequencer for key changes
  8. Select play modes (Forward, Backward, Pendulum, Probability)
  9. Press play in DAW to hear the pattern

- **Key Features**: Highlights of main functionality
  - 4 independent color channels
  - Per-color pitch sequencer with polyrhythmic loops
  - Scale sequencer for harmonic progressions
  - Multiple play modes with probability randomization
  - Visual feedback (gate flashes, velocity ripples)
  - Flexible quantization

- **Website Link**: Clickable link to https://www.touchmachines.com
- **Copyright**: © 2026 TouchMachines. All rights reserved.
- **Close Button**: Dismisses the dialog

## User Experience

- **Hover Effect**: Logo shows a subtle highlight when mouse hovers over it
- **Click to Open**: Single click on logo opens the modal dialog
- **Modal Window**: Dialog appears centered with dark theme matching plugin aesthetic
- **Easy Dismissal**: Close button or ESC key closes the dialog
- **Non-Intrusive**: Dialog doesn't interfere with plugin operation

## Technical Details

- Uses JUCE's DialogWindow for modal presentation
- Styled to match SquareBeats dark theme (0xff2a2a2a background)
- Uses AppFont system for consistent typography
- Fixed size: 600x700 pixels
- Non-resizable for consistent presentation
- Includes HyperlinkButton for website navigation

## Build Status

✅ Successfully compiled and integrated into SquareBeats VST3 plugin
✅ No breaking changes to existing functionality
✅ Ready for testing in DAW environment

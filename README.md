# SquareBeats VST3 Plugin

A VST3 MIDI sequencer plugin with a unique square-drawing interface built with JUCE.

**Version:** 1.0.0 (Release Candidate)

## What is SquareBeats?

SquareBeats is a visual MIDI sequencer where you draw squares on a 2D plane to create musical patterns. Each square represents a MIDI note - its position determines timing and pitch, while its size controls duration and velocity. With 4 independent color channels, per-color pitch sequencers, scale sequencing, and multiple play modes, SquareBeats makes it easy to create complex polyrhythmic and evolving patterns.

## Key Features

- **Visual MIDI Sequencing**: Draw squares to create patterns
- **4 Color Channels**: Independent MIDI routing and configuration
- **Per-Color Pitch Sequencers**: Each color has its own pitch modulation with independent loop length
- **Per-Color Loop Lengths**: Create polyrhythmic patterns with independent loop lengths per color
- **Scale Sequencer**: Chain multiple key/scale changes for harmonic progressions
- **Play Modes**: Forward, Backward, Pendulum, or Probability (with musical step sizes)
- **Musical Scales**: 16 scale types including Major, Minor, modes, Pentatonic, Blues
- **Preset System**: Save/load patterns with full state preservation
- **Visual Feedback**: Gate flashes, velocity ripples, beat pulse grid, and more
- **Tempo Sync**: Tight integration with DAW tempo and transport

## Quick Start

### Building

**Windows:**
```cmd
build_modern.bat
```

**macOS/Linux:**
```bash
cmake -B build -DJUCE_DIR="/path/to/JUCE"
cmake --build build --config Release --target SquareBeats_VST3 -j4
```

### Installing

**Windows:**
```cmd
xcopy /E /I /Y "build\SquareBeats_artefacts\Release\VST3\SquareBeats.vst3" "C:\vst\SquareBeats.vst3"
```

**macOS:**
```bash
cp -r "build/SquareBeats_artefacts/Release/VST3/SquareBeats.vst3" "~/Library/Audio/Plug-Ins/VST3/"
```

### Using

1. Rescan plugins in your DAW
2. Load SquareBeats on a MIDI track
3. Route MIDI to a software instrument
4. Draw squares on the sequencing plane
5. Press play in your DAW

For detailed instructions, see [BUILD_AND_TEST.md](BUILD_AND_TEST.md)



## Documentation

### User Documentation
- **[docs/USER_GUIDE.md](docs/USER_GUIDE.md)** - Complete user guide with interface overview and tips
- **[docs/FEATURES.md](docs/FEATURES.md)** - Comprehensive feature list
- **[BUILD_AND_TEST.md](BUILD_AND_TEST.md)** - Build instructions and testing checklist
- **[TESTING_RESULTS.md](TESTING_RESULTS.md)** - Testing checklist template

### Developer Documentation
- **[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)** - System architecture and design patterns
- **[docs/DEVELOPMENT.md](docs/DEVELOPMENT.md)** - Development guide and common tasks

### Specifications
- **[.kiro/specs/squarebeats-vst3/](../.kiro/specs/squarebeats-vst3/)** - Complete specification documents
  - `requirements.md` - Feature requirements
  - `design.md` - Architecture and design
  - `tasks.md` - Implementation task list

## Contributing

This project follows the spec-driven development methodology. See the task list in `.kiro/specs/squarebeats-vst3/tasks.md` for implementation details.

## License

[Add your license information here]

## Credits

Built with [JUCE](https://juce.com/) framework for cross-platform audio plugin development.

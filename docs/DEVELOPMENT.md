# SquareBeats Development Guide

## Project Structure

```
SquareBeats/
├── Source/                    # Source code
│   ├── DataStructures.h       # Core data types
│   ├── PatternModel.h/cpp     # Pattern management
│   ├── MIDIGenerator.h/cpp    # MIDI event generation
│   ├── PlaybackEngine.h/cpp   # Tempo sync & playback
│   ├── StateManager.h/cpp     # Serialization
│   ├── PresetManager.h/cpp    # Preset file management
│   ├── PluginProcessor.h/cpp  # VST3 audio processor
│   ├── PluginEditor.h/cpp     # Main editor window
│   └── [UI Components]        # Various UI components
├── docs/                      # Documentation
├── images/                    # Assets
├── CMakeLists.txt            # CMake build configuration
├── SquareBeats.jucer         # Projucer project file
└── README.md                 # Main documentation

```

## Development Environment Setup

### Prerequisites

1. **JUCE Framework** (version 7+)
   - Download from https://juce.com/
   - Extract to known location (e.g., `C:\JUCE`)
   - No installation required

2. **C++ Compiler with Build Tools**
   - **Windows**: Visual Studio 2022 (or 2019)
     - Install "Desktop development with C++" workload
   - **macOS**: Xcode 11 or later
     - Install Command Line Tools: `xcode-select --install`

3. **CMake** (version 3.15+)
   - Download from https://cmake.org/
   - Add to PATH

### Building from Source

#### Windows (Recommended)
```cmd
# Quick build with provided script
build_modern.bat

# Or manual CMake build
cmake -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR="C:\JUCE"
cmake --build build --config Release --target SquareBeats_VST3 -- /m:4
```

#### macOS/Linux
```bash
# Configure
cmake -B build -DJUCE_DIR="/path/to/JUCE"

# Build
cmake --build build --config Release --target SquareBeats_VST3 -j4
```

**Note:** macOS builds automatically create a universal binary that works on both Intel and Apple Silicon Macs.

### Installing for Testing

**Windows:**
```cmd
xcopy /E /I /Y "build\SquareBeats_artefacts\Release\VST3\SquareBeats.vst3" "C:\vst\SquareBeats.vst3"
```

**macOS:**
```bash
cp -r "build/SquareBeats_artefacts/Release/VST3/SquareBeats.vst3" "~/Library/Audio/Plug-Ins/VST3/"
```

## Code Style

### Naming Conventions
- **Classes**: PascalCase (e.g., `PatternModel`, `MIDIGenerator`)
- **Methods**: camelCase (e.g., `addSquare()`, `processBlock()`)
- **Variables**: camelCase (e.g., `loopLengthBars`, `currentPosition`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_SQUARES`, `VERSION`)
- **Private members**: camelCase (no prefix)

### File Organization
- Header files (`.h`): Class declarations, inline methods
- Implementation files (`.cpp`): Method implementations
- Test files (`.test.cpp`): Catch2 unit tests
- Standalone test files (`.standalone.test.cpp`): Assert-based tests

### JUCE Conventions
- Use JUCE types: `juce::String`, `juce::Array`, `juce::MemoryBlock`
- Individual module includes (no `JuceHeader.h`)
- JUCE naming for UI components (e.g., `Component`, `Button`, `Slider`)

## Architecture Patterns

### Model-View-Controller
- **Model**: `PatternModel` - owns all data
- **View**: UI components - display and interaction
- **Controller**: `PluginEditor`, `PlaybackEngine` - coordinate logic

### Data Flow
```
User Input → UI Component → PatternModel → PlaybackEngine → MIDI Output
                                ↓
                          StateManager → Preset File
```

### Threading
- **Audio Thread**: `processBlock()` - generates MIDI, no allocations
- **UI Thread**: Timer updates, user interaction
- **Thread Safety**: Atomic variables, lock-free FIFO for visual feedback

## Key Components

### PatternModel
Central data store for all plugin state:
- Owns all squares
- Manages color configurations
- Handles scale settings
- Provides query methods

**Adding Features:**
1. Add data members to `PatternModel`
2. Add getter/setter methods
3. Update `StateManager` serialization
4. Update UI to expose new feature

### PlaybackEngine
Handles tempo-synchronized playback:
- Reads `PatternModel` state
- Generates MIDI events in `processBlock()`
- Manages per-color playback positions
- Implements play modes

**Adding Play Modes:**
1. Add enum value to `PlayMode` in `DataStructures.h`
2. Add case in `updatePlaybackPosition()` in `PlaybackEngine.cpp`
3. Add UI button in `PlayModeControls`

### StateManager
Serializes/deserializes plugin state:
- Binary format with version number
- Validates all data on load
- Backward compatible

**Adding State Fields:**
1. Increment `VERSION` constant
2. Add write code in `saveState()`
3. Add read code in `loadState()` with version check
4. Test round-trip serialization

### UI Components
JUCE components for user interaction:
- Inherit from `juce::Component`
- Override `paint()` for rendering
- Override `resized()` for layout
- Handle mouse events

**Adding UI Components:**
1. Create `.h` and `.cpp` files
2. Add to `CMakeLists.txt`
3. Instantiate in `PluginEditor`
4. Position in `PluginEditor::resized()`

## Testing

### Unit Tests

Build and run standalone tests:
```cmd
# Compile individual test
compile_patternmodel_test.bat

# Run test executable
PatternModelTests.exe
```

Available test suites:
- `PatternModelTests.exe`: Pattern management
- `MIDIGeneratorTests.exe`: MIDI generation
- `PitchSequencerTests.exe`: Pitch sequencer
- `ConversionUtilsTests.exe`: Coordinate conversion
- `PitchSequencerIntegrationTests.exe`: Integration tests

### Manual Testing in DAW

1. Build and install plugin
2. Load in DAW (Ableton, FL Studio, Reaper, etc.)
3. Create MIDI track with SquareBeats
4. Route MIDI to software instrument
5. Test features systematically

See `BUILD_AND_TEST.md` for comprehensive testing checklist.

## Debugging

### Debug Build
```cmd
cmake --build build --config Debug --target SquareBeats_VST3
```

### Debugging in DAW
1. Build Debug configuration
2. Install debug VST3
3. Attach debugger to DAW process
4. Set breakpoints in plugin code

### Logging
Use JUCE Logger:
```cpp
DBG("Debug message: " << value);
juce::Logger::writeToLog("Log message");
```

### MIDI Monitoring
Use MIDI monitor tools:
- Windows: MIDI-OX, MIDIView
- macOS: MIDI Monitor
- Cross-platform: Protokol

## Common Tasks

### Adding a New Color Configuration Parameter

1. **Add to `ColorChannelConfig` in `DataStructures.h`:**
```cpp
struct ColorChannelConfig {
    // ... existing fields ...
    float newParameter = 0.5f;  // Add new field
};
```

2. **Update `StateManager.cpp` serialization:**
```cpp
// In saveState():
stream.writeFloat(config.newParameter);

// In loadState():
config.newParameter = stream.readFloat();
```

3. **Add UI control in `ColorConfigPanel`:**
```cpp
// In ColorConfigPanel.h:
juce::Slider newParameterSlider;

// In ColorConfigPanel.cpp:
addAndMakeVisible(newParameterSlider);
newParameterSlider.onValueChange = [this] {
    // Update model
};
```

4. **Update layout in `ColorConfigPanel::resized()`**

### Adding a New Play Mode

1. **Add enum in `DataStructures.h`:**
```cpp
enum PlayMode {
    PLAY_FORWARD,
    PLAY_BACKWARD,
    PLAY_PENDULUM,
    PLAY_PROBABILITY,
    PLAY_NEW_MODE  // Add here
};
```

2. **Implement in `PlaybackEngine.cpp`:**
```cpp
case PLAY_NEW_MODE:
    // Implement playback logic
    break;
```

3. **Add UI button in `PlayModeControls`**

### Adding a New Scale Type

1. **Add enum in `DataStructures.h`:**
```cpp
enum ScaleType {
    // ... existing scales ...
    SCALE_NEW_SCALE
};
```

2. **Add scale intervals in `ScaleConfig::snapToScale()`:**
```cpp
case SCALE_NEW_SCALE:
    intervals = {0, 2, 4, 5, 7, 9, 11};  // Example
    break;
```

3. **Add to dropdown in `ScaleControls`**

## Performance Optimization

### Audio Thread
- No allocations in `processBlock()`
- Use pre-allocated buffers
- Minimize branching
- Profile with DAW's performance monitor

### UI Thread
- Limit timer frequency (60 FPS max)
- Use dirty flags to avoid unnecessary repaints
- Batch UI updates
- Profile with graphics profiler

### Memory
- Use `std::vector` with `reserve()` for known sizes
- Avoid unnecessary copies
- Use references where possible
- Profile with memory profiler

## Release Checklist

- [ ] All unit tests pass
- [ ] Manual testing in multiple DAWs
- [ ] No memory leaks (run with sanitizers)
- [ ] No stuck notes
- [ ] Presets save/load correctly
- [ ] Cross-platform testing (Windows + macOS)
- [ ] Performance benchmarks meet targets
- [ ] Documentation updated
- [ ] Version number incremented
- [ ] Build Release configuration
- [ ] Code signing (if applicable)

## Contributing

### Workflow
1. Create feature branch from `main`
2. Implement feature with tests
3. Test manually in DAW
4. Update documentation
5. Submit pull request

### Code Review
- Check for thread safety
- Verify no allocations in audio thread
- Ensure backward compatibility
- Test preset save/load
- Verify UI responsiveness

## Resources

### JUCE Documentation
- https://docs.juce.com/
- https://forum.juce.com/

### VST3 Specification
- https://steinbergmedia.github.io/vst3_doc/

### MIDI Specification
- https://www.midi.org/specifications

### Project Documentation
- `docs/ARCHITECTURE.md`: System architecture
- `docs/FEATURES.md`: Feature list
- `docs/USER_GUIDE.md`: User documentation
- `BUILD_AND_TEST.md`: Build and testing guide

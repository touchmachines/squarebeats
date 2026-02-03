@echo off
REM Script to remove JuceHeader.h includes from source files for CMake build

echo Fixing JUCE includes for CMake build...

REM Remove #include "JuceHeader.h" from all header files
powershell -Command "(Get-Content 'Source\PluginProcessor.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\PluginProcessor.h'"
powershell -Command "(Get-Content 'Source\PluginEditor.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\PluginEditor.h'"
powershell -Command "(Get-Content 'Source\PatternModel.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\PatternModel.h'"
powershell -Command "(Get-Content 'Source\DataStructures.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\DataStructures.h'"
powershell -Command "(Get-Content 'Source\ConversionUtils.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\ConversionUtils.h'"
powershell -Command "(Get-Content 'Source\MIDIGenerator.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\MIDIGenerator.h'"
powershell -Command "(Get-Content 'Source\PlaybackEngine.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\PlaybackEngine.h'"
powershell -Command "(Get-Content 'Source\StateManager.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\StateManager.h'"
powershell -Command "(Get-Content 'Source\SequencingPlaneComponent.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\SequencingPlaneComponent.h'"
powershell -Command "(Get-Content 'Source\ColorSelectorComponent.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\ColorSelectorComponent.h'"
powershell -Command "(Get-Content 'Source\ColorConfigPanel.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\ColorConfigPanel.h'"
powershell -Command "(Get-Content 'Source\LoopLengthSelector.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\LoopLengthSelector.h'"
powershell -Command "(Get-Content 'Source\TimeSignatureControls.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\TimeSignatureControls.h'"
powershell -Command "(Get-Content 'Source\ControlButtons.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\ControlButtons.h'"
powershell -Command "(Get-Content 'Source\PitchSequencerComponent.h') -replace '#include \"JuceHeader.h\"', '' | Set-Content 'Source\PitchSequencerComponent.h'"

echo Done! JuceHeader.h includes removed.
echo You can now build with CMake.

@echo off
echo Compiling UI Components test...

REM Compile the test
g++ -std=c++17 ^
    -I"C:\JUCE\modules" ^
    -I"Source" ^
    Source/UIComponents.test.cpp ^
    Source/PatternModel.cpp ^
    Source/ColorSelectorComponent.cpp ^
    Source/ColorConfigPanel.cpp ^
    Source/LoopLengthSelector.cpp ^
    Source/TimeSignatureControls.cpp ^
    Source/ControlButtons.cpp ^
    Source/PitchSequencerComponent.cpp ^
    Source/JuceHeader.h ^
    -DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 ^
    -DJUCE_STANDALONE_APPLICATION=1 ^
    -DJUCE_PLUGINHOST_VST3=0 ^
    -DJUCE_PLUGINHOST_AU=0 ^
    -o UIComponentsTests.exe

if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo Running tests...
    UIComponentsTests.exe
) else (
    echo Compilation failed!
    exit /b 1
)

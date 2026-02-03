@echo off
echo Compiling PluginProcessor test...

REM Compile the test
g++ -std=c++17 ^
    -I"C:\JUCE\modules" ^
    -I"Source" ^
    Source/PluginProcessor.test.cpp ^
    Source/PluginProcessor.cpp ^
    Source/PluginEditor.cpp ^
    Source/PatternModel.cpp ^
    Source/MIDIGenerator.cpp ^
    Source/PlaybackEngine.cpp ^
    Source/StateManager.cpp ^
    Source/JuceHeader.h ^
    -DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 ^
    -DJUCE_STANDALONE_APPLICATION=1 ^
    -DJUCE_PLUGINHOST_VST3=0 ^
    -DJUCE_PLUGINHOST_AU=0 ^
    -o PluginProcessorTests.exe

if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo Running tests...
    PluginProcessorTests.exe
) else (
    echo Compilation failed!
    exit /b 1
)

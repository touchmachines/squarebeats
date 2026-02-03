@echo off
echo Compiling PitchSequencerComponent standalone test...

g++ -std=c++17 ^
    -I./Source ^
    -o PitchSequencerTests.exe ^
    Source/PitchSequencerComponent.standalone.test.cpp

if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo.
    echo Running tests...
    echo.
    PitchSequencerTests.exe
) else (
    echo Compilation failed!
    exit /b 1
)

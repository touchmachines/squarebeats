@echo off
echo Compiling ScaleSequencerComponent note highlighting test...

g++ -std=c++17 ^
    -I./Source ^
    -o ScaleSequencerTests.exe ^
    Source/ScaleSequencerComponent.standalone.test.cpp

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful! Running tests...
    echo.
    ScaleSequencerTests.exe
) else (
    echo.
    echo Compilation failed!
    exit /b 1
)

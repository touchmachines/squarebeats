@echo off
REM Modern JUCE VST3 Build Script
REM This script uses CMake with modern JUCE includes (no JuceHeader.h)

echo ========================================
echo Building SquareBeats VST3 Plugin
echo Using Modern JUCE 7+ Approach
echo ========================================
echo.

REM Clean previous build
if exist build (
    echo Cleaning previous build...
    rmdir /s /q build
)

REM Configure with CMake
echo Configuring CMake...
cmake -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR="C:\JUCE"
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    exit /b 1
)

echo.
echo Building Release configuration...
cmake --build build --config Release --target SquareBeats_VST3 -- /m:4
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo ========================================
echo Build Complete!
echo ========================================
echo.
echo VST3 Plugin Location:
echo build\SquareBeats_artefacts\Release\VST3\SquareBeats.vst3
echo.
echo To install, copy the .vst3 folder to your VST3 directory:
echo   Windows: C:\Program Files\Common Files\VST3\
echo.

pause

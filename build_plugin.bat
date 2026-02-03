@echo off
REM SquareBeats VST3 Plugin Build Script for Windows
REM This script builds the SquareBeats plugin using CMake

echo ========================================
echo SquareBeats VST3 Plugin Build Script
echo ========================================
echo.

REM Check if JUCE_DIR is set
if not defined JUCE_DIR (
    echo ERROR: JUCE_DIR environment variable is not set.
    echo Please set JUCE_DIR to your JUCE installation path.
    echo Example: set JUCE_DIR=C:\JUCE
    echo.
    pause
    exit /b 1
)

echo JUCE Directory: %JUCE_DIR%
echo.

REM Create build directory
if not exist build (
    echo Creating build directory...
    mkdir build
)

cd build

REM Configure CMake
echo Configuring CMake...
cmake -DJUCE_DIR=%JUCE_DIR% -G "Visual Studio 17 2022" ..
if errorlevel 1 (
    echo ERROR: CMake configuration failed.
    echo Make sure you have Visual Studio 2022 installed.
    echo If you have a different version, modify the -G parameter.
    cd ..
    pause
    exit /b 1
)

echo.
echo Building plugin (Release configuration)...
cmake --build . --config Release
if errorlevel 1 (
    echo ERROR: Build failed.
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo VST3 plugin location:
echo %CD%\SquareBeats_artefacts\Release\VST3\SquareBeats.vst3
echo.
echo To install the plugin, copy it to:
echo C:\Program Files\Common Files\VST3\
echo.
echo Would you like to copy the plugin to the VST3 folder now? (requires admin)
set /p INSTALL="Enter Y to install, N to skip: "

if /i "%INSTALL%"=="Y" (
    echo.
    echo Installing plugin...
    xcopy /Y /I "SquareBeats_artefacts\Release\VST3\SquareBeats.vst3" "C:\Program Files\Common Files\VST3\"
    if errorlevel 1 (
        echo ERROR: Installation failed. You may need to run this script as administrator.
    ) else (
        echo Plugin installed successfully!
    )
)

cd ..
echo.
echo Done!
pause

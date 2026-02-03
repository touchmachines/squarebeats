@echo off
echo Compiling StateManager standalone test...

g++ -std=c++17 ^
    -I./Source ^
    -o StateManagerTests.exe ^
    Source/StateManager.standalone.test.cpp ^
    Source/StateManager.cpp ^
    Source/PatternModel.cpp ^
    -DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=0

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful! Running tests...
    echo.
    StateManagerTests.exe
) else (
    echo.
    echo Compilation failed!
    exit /b 1
)

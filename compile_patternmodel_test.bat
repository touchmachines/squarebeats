@echo off
echo Compiling PatternModel standalone test...

g++ -std=c++17 ^
    -I./Source ^
    -o PatternModelTests.exe ^
    Source/PatternModel.standalone.test.cpp

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful! Running tests...
    echo.
    PatternModelTests.exe
) else (
    echo.
    echo Compilation failed!
    exit /b 1
)

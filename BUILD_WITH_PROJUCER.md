# Building SquareBeats with Projucer (Command Line)

## Quick Start

1. **Set JUCE path in Projucer:**
   ```cmd
   cd C:\JUCE
   Projucer.exe
   ```
   - Go to File → Global Paths
   - Set "Path to JUCE" to `C:\JUCE`
   - Close Projucer

2. **Generate build files:**
   ```cmd
   cd C:\kiro\Squarebeats\SquareBeats
   C:\JUCE\Projucer.exe --resave SquareBeats.jucer
   ```

3. **Build from command line:**
   ```cmd
   cd Builds\VisualStudio2022
   msbuild SquareBeats.sln /p:Configuration=Release /p:Platform=x64
   ```

4. **Find your VST3:**
   ```
   Builds\VisualStudio2022\x64\Release\VST3\SquareBeats.vst3
   ```

## Alternative: Use Visual Studio

After step 2, you can also:
```cmd
cd Builds\VisualStudio2022
start SquareBeats.sln
```
Then build in Visual Studio (Ctrl+Shift+B).

## Install Plugin

Copy to:
```
C:\Program Files\Common Files\VST3\SquareBeats.vst3
```

## Troubleshooting

**"Projucer command not found":**
- Use full path: `C:\JUCE\Projucer.exe`

**"msbuild not found":**
- Run from "Developer Command Prompt for VS 2022"
- Or add to PATH: `C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin`

**Build errors:**
- Make sure all source files are added to the .jucer project
- Check JUCE modules are enabled in Projucer

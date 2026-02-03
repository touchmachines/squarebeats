#!/bin/bash
# SquareBeats VST3 Plugin Build Script for macOS/Linux
# This script builds the SquareBeats plugin using CMake

echo "========================================"
echo "SquareBeats VST3 Plugin Build Script"
echo "========================================"
echo ""

# Check if JUCE_DIR is set
if [ -z "$JUCE_DIR" ]; then
    echo "ERROR: JUCE_DIR environment variable is not set."
    echo "Please set JUCE_DIR to your JUCE installation path."
    echo "Example: export JUCE_DIR=/path/to/JUCE"
    echo ""
    exit 1
fi

echo "JUCE Directory: $JUCE_DIR"
echo ""

# Create build directory
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Configure CMake
echo "Configuring CMake..."
cmake -DJUCE_DIR="$JUCE_DIR" -DCMAKE_BUILD_TYPE=Release ..
if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed."
    cd ..
    exit 1
fi

echo ""
echo "Building plugin (Release configuration)..."
cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed."
    cd ..
    exit 1
fi

echo ""
echo "========================================"
echo "Build completed successfully!"
echo "========================================"
echo ""
echo "VST3 plugin location:"
echo "$(pwd)/SquareBeats_artefacts/Release/VST3/SquareBeats.vst3"
echo ""

# Determine installation path based on OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    INSTALL_PATH="$HOME/Library/Audio/Plug-Ins/VST3/"
    echo "To install the plugin, copy it to:"
    echo "$INSTALL_PATH"
    echo ""
    read -p "Would you like to copy the plugin to the VST3 folder now? (y/n): " INSTALL
    
    if [[ "$INSTALL" == "y" || "$INSTALL" == "Y" ]]; then
        echo ""
        echo "Installing plugin..."
        mkdir -p "$INSTALL_PATH"
        cp -R "SquareBeats_artefacts/Release/VST3/SquareBeats.vst3" "$INSTALL_PATH"
        if [ $? -eq 0 ]; then
            echo "Plugin installed successfully!"
        else
            echo "ERROR: Installation failed."
        fi
    fi
else
    # Linux
    INSTALL_PATH="$HOME/.vst3/"
    echo "To install the plugin, copy it to:"
    echo "$INSTALL_PATH"
    echo ""
    read -p "Would you like to copy the plugin to the VST3 folder now? (y/n): " INSTALL
    
    if [[ "$INSTALL" == "y" || "$INSTALL" == "Y" ]]; then
        echo ""
        echo "Installing plugin..."
        mkdir -p "$INSTALL_PATH"
        cp -R "SquareBeats_artefacts/Release/VST3/SquareBeats.vst3" "$INSTALL_PATH"
        if [ $? -eq 0 ]; then
            echo "Plugin installed successfully!"
        else
            echo "ERROR: Installation failed."
        fi
    fi
fi

cd ..
echo ""
echo "Done!"

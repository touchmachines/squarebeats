#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
PatternModel::PatternModel()
    : loopLengthBars(2)
    , timeSignature(4, 4)
    , nextUniqueId(1)
{
    initializeDefaultColorConfigs();
}

//==============================================================================
// Square management methods

Square* PatternModel::createSquare(float left, float top, float width, float height, int colorId)
{
    // Define minimum square size to prevent zero-width/height squares
    constexpr float MIN_SIZE = 0.01f;
    
    // Clamp coordinates to valid range [0.0, 1.0]
    left = juce::jlimit(0.0f, 1.0f, left);
    top = juce::jlimit(0.0f, 1.0f, top);
    width = juce::jlimit(MIN_SIZE, 1.0f - left, width);
    height = juce::jlimit(MIN_SIZE, 1.0f - top, height);
    
    // Clamp color ID to valid range [0, 3]
    colorId = juce::jlimit(0, 3, colorId);
    
    // Create square with unique ID
    uint32_t id = nextUniqueId++;
    squares.emplace_back(left, top, width, height, colorId, id);
    
    sendChangeMessage();
    return &squares.back();
}

bool PatternModel::deleteSquare(uint32_t squareId)
{
    auto it = findSquareById(squareId);
    if (it != squares.end())
    {
        squares.erase(it);
        sendChangeMessage();
        return true;
    }
    return false;
}

bool PatternModel::moveSquare(uint32_t squareId, float newLeft, float newTop)
{
    auto it = findSquareById(squareId);
    if (it != squares.end())
    {
        // Clamp to valid range
        it->leftEdge = juce::jlimit(0.0f, 1.0f, newLeft);
        it->topEdge = juce::jlimit(0.0f, 1.0f, newTop);
        sendChangeMessage();
        return true;
    }
    return false;
}

bool PatternModel::resizeSquare(uint32_t squareId, float newWidth, float newHeight)
{
    // Define minimum square size to prevent zero-width/height squares
    constexpr float MIN_SIZE = 0.01f;
    
    auto it = findSquareById(squareId);
    if (it != squares.end())
    {
        // Clamp to valid range, ensuring square doesn't extend beyond bounds
        it->width = juce::jlimit(MIN_SIZE, 1.0f - it->leftEdge, newWidth);
        it->height = juce::jlimit(MIN_SIZE, 1.0f - it->topEdge, newHeight);
        sendChangeMessage();
        return true;
    }
    return false;
}

void PatternModel::clearColorChannel(int colorId)
{
    // Remove all squares with the specified color channel ID
    squares.erase(
        std::remove_if(squares.begin(), squares.end(),
            [colorId](const Square& square) {
                return square.colorChannelId == colorId;
            }),
        squares.end()
    );
    sendChangeMessage();
}

//==============================================================================
// Query methods

std::vector<Square*> PatternModel::getSquaresInTimeRange(float startTime, float endTime)
{
    std::vector<Square*> result;
    
    for (auto& square : squares)
    {
        float squareStart = square.leftEdge;
        float squareEnd = square.getRightEdge();
        
        // Check if square's time range intersects [startTime, endTime]
        if (squareStart < endTime && squareEnd > startTime)
        {
            result.push_back(&square);
        }
    }
    
    return result;
}

std::vector<Square*> PatternModel::getAllSquares()
{
    std::vector<Square*> result;
    result.reserve(squares.size());
    
    for (auto& square : squares)
    {
        result.push_back(&square);
    }
    
    return result;
}

std::vector<const Square*> PatternModel::getAllSquares() const
{
    std::vector<const Square*> result;
    result.reserve(squares.size());
    
    for (const auto& square : squares)
    {
        result.push_back(&square);
    }
    
    return result;
}

//==============================================================================
// Color channel configuration

ColorChannelConfig& PatternModel::getColorConfig(int colorId)
{
    colorId = juce::jlimit(0, 3, colorId);
    return colorConfigs[colorId];
}

const ColorChannelConfig& PatternModel::getColorConfig(int colorId) const
{
    colorId = juce::jlimit(0, 3, colorId);
    return colorConfigs[colorId];
}

void PatternModel::setColorConfig(int colorId, const ColorChannelConfig& config)
{
    colorId = juce::jlimit(0, 3, colorId);
    
    // Create a validated copy of the config with clamped values
    ColorChannelConfig validatedConfig = config;
    
    // Clamp MIDI channel to valid range [1, 16]
    validatedConfig.midiChannel = juce::jlimit(1, 16, config.midiChannel);
    
    // Clamp MIDI note numbers to valid range [0, 127]
    validatedConfig.highNote = juce::jlimit(0, 127, config.highNote);
    validatedConfig.lowNote = juce::jlimit(0, 127, config.lowNote);
    
    colorConfigs[colorId] = validatedConfig;
    sendChangeMessage();
}

//==============================================================================
// Pitch sequencer

PitchSequencer& PatternModel::getPitchSequencer()
{
    return pitchSequencer;
}

const PitchSequencer& PatternModel::getPitchSequencer() const
{
    return pitchSequencer;
}

//==============================================================================
// Play mode configuration

PlayModeConfig& PatternModel::getPlayModeConfig()
{
    return playModeConfig;
}

const PlayModeConfig& PatternModel::getPlayModeConfig() const
{
    return playModeConfig;
}

//==============================================================================
// Scale sequencer configuration

ScaleSequencerConfig& PatternModel::getScaleSequencer()
{
    return scaleSequencer;
}

const ScaleSequencerConfig& PatternModel::getScaleSequencer() const
{
    return scaleSequencer;
}

ScaleConfig PatternModel::getActiveScale(double positionBars) const
{
    if (scaleSequencer.enabled && !scaleSequencer.segments.empty()) {
        return scaleSequencer.getScaleAtPosition(positionBars);
    }
    return scaleConfig;
}

//==============================================================================
// Global settings

void PatternModel::setLoopLength(int bars)
{
    // Clamp to valid range (1-64 bars)
    loopLengthBars = juce::jlimit(1, 64, bars);
    
    // Loop length only affects playback speed, not the squares themselves
    // Squares always use normalized coordinates (0.0 to 1.0) across the full plane
    
    sendChangeMessage();
}

int PatternModel::getLoopLength() const
{
    return loopLengthBars;
}

void PatternModel::setTimeSignature(int numerator, int denominator)
{
    // Clamp numerator to valid range [1, 16]
    numerator = juce::jlimit(1, 16, numerator);
    
    // Clamp denominator to valid values (1, 2, 4, 8, 16)
    if (denominator <= 1)
        denominator = 1;
    else if (denominator <= 2)
        denominator = 2;
    else if (denominator <= 4)
        denominator = 4;
    else if (denominator <= 8)
        denominator = 8;
    else
        denominator = 16;
    
    timeSignature = TimeSignature(numerator, denominator);
    sendChangeMessage();
}

TimeSignature PatternModel::getTimeSignature() const
{
    return timeSignature;
}

void PatternModel::setScaleConfig(const ScaleConfig& config)
{
    scaleConfig = config;
    sendChangeMessage();
}

const ScaleConfig& PatternModel::getScaleConfig() const
{
    return scaleConfig;
}

//==============================================================================
// Helper methods

std::vector<Square>::iterator PatternModel::findSquareById(uint32_t squareId)
{
    return std::find_if(squares.begin(), squares.end(),
        [squareId](const Square& square) {
            return square.uniqueId == squareId;
        });
}

void PatternModel::initializeDefaultColorConfigs()
{
    // Default waveform resolution
    const int waveformResolution = 256;
    
    // Initialize 4 color channels with color-blind friendly metallic colors
    
    // Channel 0: Copper/Rose Gold - warm metallic
    colorConfigs[0].midiChannel = 1;
    colorConfigs[0].highNote = 84;  // C6
    colorConfigs[0].lowNote = 48;   // C3
    colorConfigs[0].quantize = Q_1_16;
    colorConfigs[0].displayColor = juce::Colour(0xFFE8A87C);  // Copper/Rose Gold
    colorConfigs[0].pitchWaveform.resize(waveformResolution, 0.0f);
    
    // Channel 1: Steel Blue - cool metallic, high contrast with copper
    colorConfigs[1].midiChannel = 2;
    colorConfigs[1].highNote = 84;  // C6
    colorConfigs[1].lowNote = 48;   // C3
    colorConfigs[1].quantize = Q_1_16;
    colorConfigs[1].displayColor = juce::Colour(0xFF85C1E9);  // Steel Blue
    colorConfigs[1].pitchWaveform.resize(waveformResolution, 0.0f);
    
    // Channel 2: Deep Purple/Violet - distinct from blue, good saturation
    colorConfigs[2].midiChannel = 3;
    colorConfigs[2].highNote = 84;  // C6
    colorConfigs[2].lowNote = 48;   // C3
    colorConfigs[2].quantize = Q_1_16;
    colorConfigs[2].displayColor = juce::Colour(0xFFAF7AC5);  // Deep Purple
    colorConfigs[2].pitchWaveform.resize(waveformResolution, 0.0f);
    
    // Channel 3: Bright Cyan/Teal - high brightness, distinct from purple
    colorConfigs[3].midiChannel = 4;
    colorConfigs[3].highNote = 84;  // C6
    colorConfigs[3].lowNote = 48;   // C3
    colorConfigs[3].quantize = Q_1_16;
    colorConfigs[3].displayColor = juce::Colour(0xFF48C9B0);  // Bright Teal
    colorConfigs[3].pitchWaveform.resize(waveformResolution, 0.0f);
}

} // namespace SquareBeats

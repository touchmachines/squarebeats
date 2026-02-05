#include "StateManager.h"

namespace SquareBeats {

//==============================================================================
void StateManager::saveState(const PatternModel& model, juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, false);
    
    // Write magic number and version
    stream.writeInt(MAGIC_NUMBER);
    stream.writeInt(VERSION);
    
    // Write global settings
    stream.writeDouble(model.getLoopLength());
    
    TimeSignature timeSig = model.getTimeSignature();
    stream.writeInt(timeSig.numerator);
    stream.writeInt(timeSig.denominator);
    
    // Write squares
    auto squares = model.getAllSquares();
    stream.writeInt(static_cast<int>(squares.size()));
    
    for (const Square* square : squares)
    {
        stream.writeFloat(square->leftEdge);
        stream.writeFloat(square->width);
        stream.writeFloat(square->topEdge);
        stream.writeFloat(square->height);
        stream.writeInt(square->colorChannelId);
        stream.writeInt(static_cast<int>(square->uniqueId));
    }
    
    // Write color channel configurations (4 channels) with per-color pitch waveforms
    for (int i = 0; i < 4; ++i)
    {
        const ColorChannelConfig& config = model.getColorConfig(i);
        stream.writeInt(config.midiChannel);
        stream.writeInt(config.highNote);
        stream.writeInt(config.lowNote);
        stream.writeInt(static_cast<int>(config.quantize));
        stream.writeInt(static_cast<int>(config.displayColor.getARGB()));
        stream.writeInt(config.pitchSeqLoopLengthBars);
        stream.writeDouble(config.mainLoopLengthBars);  // Per-color loop length
        
        // Write per-color pitch waveform
        stream.writeInt(static_cast<int>(config.pitchWaveform.size()));
        for (float value : config.pitchWaveform)
        {
            stream.writeFloat(value);
        }
    }
    
    // Write pitch sequencer global settings (editing mode)
    const PitchSequencer& pitchSeq = model.getPitchSequencer();
    stream.writeBool(pitchSeq.editingPitch);
    
    // Write scale configuration (Version 4+)
    const ScaleConfig& scaleConfig = model.getScaleConfig();
    stream.writeInt(static_cast<int>(scaleConfig.rootNote));
    stream.writeInt(static_cast<int>(scaleConfig.scaleType));
    
    // Write scale sequencer configuration (Version 5+)
    const ScaleSequencerConfig& scaleSeqConfig = model.getScaleSequencer();
    stream.writeBool(scaleSeqConfig.enabled);
    stream.writeInt(static_cast<int>(scaleSeqConfig.segments.size()));
    
    for (const auto& segment : scaleSeqConfig.segments)
    {
        stream.writeInt(static_cast<int>(segment.rootNote));
        stream.writeInt(static_cast<int>(segment.scaleType));
        stream.writeInt(segment.lengthBars);
    }
    
    // Write play mode configuration (Version 7+)
    const PlayModeConfig& playModeConfig = model.getPlayModeConfig();
    stream.writeInt(static_cast<int>(playModeConfig.mode));
    stream.writeFloat(playModeConfig.stepJumpSize);
    stream.writeFloat(playModeConfig.probability);
    // Note: pendulumForward is internal state, not saved (always starts forward)
}

//==============================================================================
bool StateManager::loadState(PatternModel& model, const void* data, int sizeInBytes)
{
    // Validate input parameters
    if (data == nullptr || sizeInBytes < 8)
    {
        // Not enough data for magic number and version
        juce::Logger::writeToLog("StateManager: Invalid input data (null or too small)");
        return false;
    }
    
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    
    // Validate magic number
    uint32_t magic = static_cast<uint32_t>(stream.readInt());
    if (magic != MAGIC_NUMBER)
    {
        juce::Logger::writeToLog("StateManager: Invalid magic number (expected 0x" + 
                                juce::String::toHexString(static_cast<int>(MAGIC_NUMBER)) + 
                                ", got 0x" + juce::String::toHexString(static_cast<int>(magic)) + ")");
        return false;
    }
    
    // Validate version (support versions 3 and 4)
    uint32_t version = static_cast<uint32_t>(stream.readInt());
    if (version < 3 || version > VERSION)
    {
        juce::Logger::writeToLog("StateManager: Unsupported version " + juce::String(version) + 
                                " (expected 3-" + juce::String(VERSION) + ")");
        return false;
    }
    
    try
    {
        // Check if we have enough data remaining
        if (stream.getNumBytesRemaining() < 16)  // Need 1 double (8 bytes) + 2 ints (8 bytes) for global settings
        {
            juce::Logger::writeToLog("StateManager: Truncated data (not enough for global settings)");
            return false;
        }
        
        // Read global settings
        double loopLength = stream.readDouble();
        // Time signature is always 4/4 now - read and discard for backward compatibility
        stream.readInt();  // timeSigNumerator (ignored)
        stream.readInt();  // timeSigDenominator (ignored)
        
        // Validate loop length before applying
        if (loopLength < 1.0 / 16.0 || loopLength > 64.0)
        {
            juce::Logger::writeToLog("StateManager: Invalid loop length " + juce::String(loopLength) + ", using default");
            loopLength = 2.0;  // Default to 2 bars
        }
        
        model.setLoopLength(loopLength);
        model.setTimeSignature(4, 4);  // Always use 4/4
        
        // Check if we have enough data for square count
        if (stream.getNumBytesRemaining() < 4)
        {
            juce::Logger::writeToLog("StateManager: Truncated data (not enough for square count)");
            return false;
        }
        
        // Read squares
        int numSquares = stream.readInt();
        if (numSquares < 0 || numSquares > 100000)
        {
            // Sanity check: unreasonable number of squares
            juce::Logger::writeToLog("StateManager: Invalid number of squares: " + juce::String(numSquares));
            return false;
        }
        
        // Clear existing squares before loading
        for (int i = 0; i < 4; ++i)
        {
            model.clearColorChannel(i);
        }
        
        int squaresLoaded = 0;
        for (int i = 0; i < numSquares; ++i)
        {
            // Check if we have enough data for this square (6 values: 4 floats + 2 ints)
            if (stream.getNumBytesRemaining() < 24)
            {
                juce::Logger::writeToLog("StateManager: Truncated data while reading squares (loaded " + 
                                        juce::String(squaresLoaded) + " of " + juce::String(numSquares) + ")");
                break;  // Stop loading squares but continue with rest of state
            }
            
            float leftEdge = stream.readFloat();
            float width = stream.readFloat();
            float topEdge = stream.readFloat();
            float height = stream.readFloat();
            int colorChannelId = stream.readInt();
            int uniqueId = stream.readInt();
            (void)uniqueId; // Unused - new ID will be assigned
            
            // Validate square data
            if (std::isnan(leftEdge) || std::isnan(width) || 
                std::isnan(topEdge) || std::isnan(height))
            {
                juce::Logger::writeToLog("StateManager: Invalid square coordinates (NaN) at index " + juce::String(i));
                continue; // Skip this square but continue loading
            }
            
            if (std::isinf(leftEdge) || std::isinf(width) || 
                std::isinf(topEdge) || std::isinf(height))
            {
                juce::Logger::writeToLog("StateManager: Invalid square coordinates (Inf) at index " + juce::String(i));
                continue; // Skip this square but continue loading
            }
            
            // Create square (PatternModel will clamp values to valid ranges)
            model.createSquare(leftEdge, topEdge, width, height, colorChannelId);
            squaresLoaded++;
        }
        
        // Read color channel configurations with per-color pitch waveforms
        for (int i = 0; i < 4; ++i)
        {
            int minBytes = (version >= 6) ? 32 : 24;  // Version 6+ has mainLoopLengthBars (double)
            if (stream.getNumBytesRemaining() < minBytes)
            {
                juce::Logger::writeToLog("StateManager: Truncated data (not enough for color config " + juce::String(i) + ")");
                break;
            }
            
            ColorChannelConfig config;
            config.midiChannel = stream.readInt();
            config.highNote = stream.readInt();
            config.lowNote = stream.readInt();
            config.quantize = static_cast<QuantizationValue>(stream.readInt());
            config.displayColor = juce::Colour(static_cast<uint32_t>(stream.readInt()));
            config.pitchSeqLoopLengthBars = juce::jlimit(1, 64, stream.readInt());
            
            // Per-color loop length (Version 6+)
            if (version >= 6)
            {
                config.mainLoopLengthBars = stream.readDouble();
            }
            else
            {
                config.mainLoopLengthBars = 0.0;  // Default to global
            }
            
            // Read per-color pitch waveform
            if (stream.getNumBytesRemaining() < 4)
            {
                juce::Logger::writeToLog("StateManager: Truncated data (not enough for waveform size)");
                break;
            }
            
            int waveformSize = stream.readInt();
            if (waveformSize < 0 || waveformSize > 1000000)
            {
                juce::Logger::writeToLog("StateManager: Invalid waveform size: " + juce::String(waveformSize));
                waveformSize = 0;
            }
            
            if (stream.getNumBytesRemaining() < waveformSize * 4)
            {
                juce::Logger::writeToLog("StateManager: Truncated data (not enough for waveform data)");
                waveformSize = static_cast<int>(stream.getNumBytesRemaining() / 4);
            }
            
            config.pitchWaveform.clear();
            config.pitchWaveform.reserve(waveformSize);
            for (int j = 0; j < waveformSize; ++j)
            {
                float value = stream.readFloat();
                if (!std::isnan(value) && !std::isinf(value))
                {
                    config.pitchWaveform.push_back(value);
                }
                else
                {
                    config.pitchWaveform.push_back(0.0f);
                }
            }
            
            // If waveform is empty, initialize with default size
            if (config.pitchWaveform.empty())
            {
                config.pitchWaveform.resize(256, 0.0f);
            }
            
            // Validate and clamp values
            config.midiChannel = juce::jlimit(1, 16, config.midiChannel);
            config.highNote = juce::jlimit(0, 127, config.highNote);
            config.lowNote = juce::jlimit(0, 127, config.lowNote);
            
            // Validate quantization value
            if (config.quantize < Q_1_32 || config.quantize > Q_1_BAR)
            {
                config.quantize = Q_1_16;
            }
            
            model.setColorConfig(i, config);
        }
        
        // Read pitch sequencer global settings (editing mode)
        if (stream.getNumBytesRemaining() >= 1)
        {
            bool pitchSeqEditing = stream.readBool();
            
            PitchSequencer& pitchSeq = model.getPitchSequencer();
            pitchSeq.editingPitch = pitchSeqEditing;
        }
        
        // Read scale configuration (Version 4+)
        if (version >= 4 && stream.getNumBytesRemaining() >= 8)
        {
            int rootNote = stream.readInt();
            int scaleType = stream.readInt();
            
            ScaleConfig scaleConfig;
            scaleConfig.rootNote = static_cast<RootNote>(juce::jlimit(0, 11, rootNote));
            scaleConfig.scaleType = static_cast<ScaleType>(juce::jlimit(0, static_cast<int>(NUM_SCALE_TYPES) - 1, scaleType));
            model.setScaleConfig(scaleConfig);
        }
        
        // Read scale sequencer configuration (Version 5+)
        if (version >= 5 && stream.getNumBytesRemaining() >= 5)  // 1 bool + 1 int minimum
        {
            bool scaleSeqEnabled = stream.readBool();
            int numSegments = stream.readInt();
            
            // Validate segment count
            if (numSegments < 0 || numSegments > ScaleSequencerConfig::MAX_SEGMENTS)
            {
                juce::Logger::writeToLog("StateManager: Invalid scale sequencer segment count: " + juce::String(numSegments));
                numSegments = juce::jlimit(0, ScaleSequencerConfig::MAX_SEGMENTS, numSegments);
            }
            
            ScaleSequencerConfig scaleSeqConfig;
            scaleSeqConfig.enabled = scaleSeqEnabled;
            scaleSeqConfig.segments.clear();
            
            for (int i = 0; i < numSegments; ++i)
            {
                if (stream.getNumBytesRemaining() < 12)  // 3 ints per segment
                {
                    juce::Logger::writeToLog("StateManager: Truncated data while reading scale sequencer segments");
                    break;
                }
                
                int rootNote = stream.readInt();
                int scaleType = stream.readInt();
                int lengthBars = stream.readInt();
                
                // Validate and clamp values
                rootNote = juce::jlimit(0, 11, rootNote);
                scaleType = juce::jlimit(0, static_cast<int>(NUM_SCALE_TYPES) - 1, scaleType);
                lengthBars = juce::jlimit(1, 16, lengthBars);
                
                ScaleSequenceSegment segment(
                    static_cast<RootNote>(rootNote),
                    static_cast<ScaleType>(scaleType),
                    lengthBars
                );
                scaleSeqConfig.segments.push_back(segment);
            }
            
            // Ensure at least one segment exists
            if (scaleSeqConfig.segments.empty())
            {
                scaleSeqConfig.segments.push_back(ScaleSequenceSegment(ROOT_C, SCALE_MAJOR, 4));
            }
            
            // Use mutable getter to assign the loaded config
            model.getScaleSequencer() = scaleSeqConfig;
        }
        
        // Read play mode configuration (Version 7+)
        if (version >= 7 && stream.getNumBytesRemaining() >= 12)  // 1 int + 2 floats
        {
            int playMode = stream.readInt();
            float stepJumpSize = stream.readFloat();
            float probability = stream.readFloat();
            
            // Validate and clamp values
            playMode = juce::jlimit(0, static_cast<int>(NUM_PLAY_MODES) - 1, playMode);
            stepJumpSize = juce::jlimit(0.0f, 1.0f, stepJumpSize);
            probability = juce::jlimit(0.0f, 1.0f, probability);
            
            PlayModeConfig& playModeConfig = model.getPlayModeConfig();
            playModeConfig.mode = static_cast<PlayMode>(playMode);
            playModeConfig.stepJumpSize = stepJumpSize;
            playModeConfig.probability = probability;
            playModeConfig.pendulumForward = true;  // Always start forward
        }
        
        return true;
    }
    catch (const std::exception& e)
    {
        juce::Logger::writeToLog("StateManager: Exception during deserialization: " + juce::String(e.what()));
        return false;
    }
    catch (...)
    {
        juce::Logger::writeToLog("StateManager: Unknown exception during deserialization");
        return false;
    }
}

} // namespace SquareBeats

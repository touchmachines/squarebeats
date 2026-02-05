#include "PresetManager.h"
#include "StateManager.h"

namespace SquareBeats {

//==============================================================================
PresetManager::PresetManager()
{
    // Determine the standard VST3 preset location based on platform
#if JUCE_WINDOWS
    // Windows: Documents/VST3 Presets/Touchmachines/SquareBeats/
    auto documentsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    presetDirectory = documentsDir.getChildFile("VST3 Presets")
                                   .getChildFile("Touchmachines")
                                   .getChildFile("SquareBeats");
#elif JUCE_MAC
    // macOS: /Library/Audio/Presets/Touchmachines/SquareBeats/
    presetDirectory = juce::File("/Library/Audio/Presets")
                          .getChildFile("Touchmachines")
                          .getChildFile("SquareBeats");
#elif JUCE_LINUX
    // Linux: ~/.vst3/presets/Touchmachines/SquareBeats/
    auto homeDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory);
    presetDirectory = homeDir.getChildFile(".vst3")
                             .getChildFile("presets")
                             .getChildFile("Touchmachines")
                             .getChildFile("SquareBeats");
#else
    // Fallback: use user application data directory
    auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    presetDirectory = appDataDir.getChildFile("SquareBeats")
                                 .getChildFile("Presets");
#endif
    
    ensurePresetDirectoryExists();
}

//==============================================================================
void PresetManager::ensurePresetDirectoryExists()
{
    if (!presetDirectory.exists())
    {
        auto result = presetDirectory.createDirectory();
        if (result.failed())
        {
            juce::Logger::writeToLog("PresetManager: Failed to create preset directory: " + 
                                    result.getErrorMessage());
        }
    }
}

//==============================================================================
juce::File PresetManager::getPresetFile(const juce::String& presetName) const
{
    return presetDirectory.getChildFile(presetName + ".vstpreset");
}

//==============================================================================
juce::StringArray PresetManager::getPresetList() const
{
    juce::StringArray presetNames;
    
    if (!presetDirectory.exists())
        return presetNames;
    
    // Find all .vstpreset files
    juce::Array<juce::File> presetFiles;
    presetDirectory.findChildFiles(presetFiles, 
                                   juce::File::findFiles, 
                                   false,  // non-recursive
                                   "*.vstpreset");
    
    // Extract names without extension
    for (const auto& file : presetFiles)
    {
        presetNames.add(file.getFileNameWithoutExtension());
    }
    
    // Sort alphabetically
    presetNames.sort(true);  // case-insensitive
    
    return presetNames;
}

//==============================================================================
bool PresetManager::savePreset(const PatternModel& model, const juce::String& presetName)
{
    if (presetName.isEmpty())
    {
        juce::Logger::writeToLog("PresetManager: Cannot save preset with empty name");
        return false;
    }
    
    // Ensure directory exists
    ensurePresetDirectoryExists();
    
    // Serialize the pattern model using StateManager
    juce::MemoryBlock stateData;
    StateManager::saveState(model, stateData);
    
    // Get the preset file
    auto presetFile = getPresetFile(presetName);
    
    // Write to file
    if (presetFile.replaceWithData(stateData.getData(), stateData.getSize()))
    {
        juce::Logger::writeToLog("PresetManager: Saved preset '" + presetName + "' to " + 
                                presetFile.getFullPathName());
        return true;
    }
    else
    {
        juce::Logger::writeToLog("PresetManager: Failed to save preset '" + presetName + "'");
        return false;
    }
}

//==============================================================================
bool PresetManager::loadPreset(PatternModel& model, const juce::String& presetName)
{
    if (presetName.isEmpty())
    {
        juce::Logger::writeToLog("PresetManager: Cannot load preset with empty name");
        return false;
    }
    
    auto presetFile = getPresetFile(presetName);
    
    if (!presetFile.existsAsFile())
    {
        juce::Logger::writeToLog("PresetManager: Preset file not found: " + 
                                presetFile.getFullPathName());
        return false;
    }
    
    // Read the file
    juce::MemoryBlock fileData;
    if (!presetFile.loadFileAsData(fileData))
    {
        juce::Logger::writeToLog("PresetManager: Failed to read preset file: " + 
                                presetFile.getFullPathName());
        return false;
    }
    
    // Deserialize using StateManager
    if (StateManager::loadState(model, fileData.getData(), static_cast<int>(fileData.getSize())))
    {
        juce::Logger::writeToLog("PresetManager: Loaded preset '" + presetName + "'");
        model.sendChangeMessage();  // Notify UI of changes
        return true;
    }
    else
    {
        juce::Logger::writeToLog("PresetManager: Failed to deserialize preset '" + presetName + "'");
        return false;
    }
}

//==============================================================================
bool PresetManager::deletePreset(const juce::String& presetName)
{
    if (presetName.isEmpty())
    {
        juce::Logger::writeToLog("PresetManager: Cannot delete preset with empty name");
        return false;
    }
    
    auto presetFile = getPresetFile(presetName);
    
    if (!presetFile.existsAsFile())
    {
        juce::Logger::writeToLog("PresetManager: Preset file not found: " + 
                                presetFile.getFullPathName());
        return false;
    }
    
    if (presetFile.deleteFile())
    {
        juce::Logger::writeToLog("PresetManager: Deleted preset '" + presetName + "'");
        return true;
    }
    else
    {
        juce::Logger::writeToLog("PresetManager: Failed to delete preset '" + presetName + "'");
        return false;
    }
}

//==============================================================================
bool PresetManager::presetExists(const juce::String& presetName) const
{
    return getPresetFile(presetName).existsAsFile();
}

//==============================================================================
juce::File PresetManager::getPresetDirectory() const
{
    return presetDirectory;
}

//==============================================================================
void PresetManager::createFactoryPresetsIfNeeded()
{
    // Create "_Init" preset if it doesn't exist (underscore prefix sorts to top)
    if (!presetExists("_Init"))
    {
        createInitPreset();
    }
}

//==============================================================================
void PresetManager::createInitPreset()
{
    // Create a fresh pattern model with default settings
    PatternModel initModel;
    
    // Set everything to defaults (1 bar loop, 4/4 time, no squares)
    initModel.setLoopLength(1.0);  // 1 bar
    initModel.setTimeSignature(4, 4);
    
    // Clear all squares (already empty, but be explicit)
    for (int i = 0; i < 4; ++i)
    {
        initModel.clearColorChannel(i);
    }
    
    // Set default color configs
    for (int i = 0; i < 4; ++i)
    {
        ColorChannelConfig config = initModel.getColorConfig(i);
        // Keep default values (already set by PatternModel constructor)
        // Just ensure pitch waveform is zeroed
        std::fill(config.pitchWaveform.begin(), config.pitchWaveform.end(), 0.0f);
        config.pitchSeqLoopLengthBars = 1;  // 1 bar
        config.mainLoopLengthBars = 0.0;  // Use global
        initModel.setColorConfig(i, config);
    }
    
    // Set default scale (C Major)
    ScaleConfig scaleConfig;
    scaleConfig.rootNote = ROOT_C;
    scaleConfig.scaleType = SCALE_MAJOR;
    initModel.setScaleConfig(scaleConfig);
    
    // Disable scale sequencer
    ScaleSequencerConfig& scaleSeqConfig = initModel.getScaleSequencer();
    scaleSeqConfig.enabled = false;
    scaleSeqConfig.segments.clear();
    scaleSeqConfig.segments.push_back(ScaleSequenceSegment(ROOT_C, SCALE_MAJOR, 4));
    
    // Set default play mode (Forward)
    PlayModeConfig& playModeConfig = initModel.getPlayModeConfig();
    playModeConfig.mode = PLAY_FORWARD;
    playModeConfig.stepJumpSize = 0.25f;  // 4 steps
    playModeConfig.probability = 0.5f;
    playModeConfig.pendulumForward = true;
    
    // Save as "_Init" preset (underscore prefix sorts to top)
    if (savePreset(initModel, "_Init"))
    {
        juce::Logger::writeToLog("PresetManager: Created factory preset '_Init'");
    }
    else
    {
        juce::Logger::writeToLog("PresetManager: Failed to create factory preset '_Init'");
    }
}

} // namespace SquareBeats

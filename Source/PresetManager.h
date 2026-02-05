#pragma once

#include <juce_core/juce_core.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * PresetManager handles VST3 preset file management
 * 
 * Manages .vstpreset files in the standard VST3 preset location:
 * - Windows: Documents/VST3 Presets/Touchmachines/SquareBeats/
 * - macOS: /Library/Audio/Presets/Touchmachines/SquareBeats/
 * - Linux: ~/.vst3/presets/Touchmachines/SquareBeats/
 * 
 * Features:
 * - Save/load user presets
 * - List available presets
 * - Delete presets
 * - Factory preset support
 */
class PresetManager {
public:
    PresetManager();
    ~PresetManager() = default;
    
    /**
     * Get list of all available preset names (without .vstpreset extension)
     * Includes both factory and user presets
     */
    juce::StringArray getPresetList() const;
    
    /**
     * Save current pattern model state as a named preset
     * @param model The pattern model to save
     * @param presetName Name for the preset (without extension)
     * @return true if save succeeded
     */
    bool savePreset(const PatternModel& model, const juce::String& presetName);
    
    /**
     * Load a preset by name into the pattern model
     * @param model The pattern model to load into
     * @param presetName Name of the preset to load (without extension)
     * @return true if load succeeded
     */
    bool loadPreset(PatternModel& model, const juce::String& presetName);
    
    /**
     * Delete a preset by name
     * @param presetName Name of the preset to delete (without extension)
     * @return true if delete succeeded
     */
    bool deletePreset(const juce::String& presetName);
    
    /**
     * Check if a preset name already exists
     */
    bool presetExists(const juce::String& presetName) const;
    
    /**
     * Get the preset directory path
     */
    juce::File getPresetDirectory() const;
    
    /**
     * Create factory presets if they don't exist
     */
    void createFactoryPresetsIfNeeded();
    
private:
    juce::File presetDirectory;
    
    /**
     * Ensure the preset directory exists
     */
    void ensurePresetDirectoryExists();
    
    /**
     * Get the full file path for a preset name
     */
    juce::File getPresetFile(const juce::String& presetName) const;
    
    /**
     * Create the "Init" factory preset
     */
    void createInitPreset();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};

} // namespace SquareBeats

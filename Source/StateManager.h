#pragma once

#include <juce_core/juce_core.h>
#include "PatternModel.h"

namespace SquareBeats {

//==============================================================================
/**
 * StateManager handles serialization and deserialization of PatternModel
 * for VST3 state persistence (preset saving/loading)
 */
class StateManager {
public:
    /**
     * Serialize a PatternModel to binary format
     * @param model The pattern model to serialize
     * @param destData Output memory block to write serialized data
     */
    static void saveState(const PatternModel& model, juce::MemoryBlock& destData);
    
    /**
     * Deserialize binary data to restore a PatternModel
     * @param model The pattern model to restore into
     * @param data Pointer to serialized data
     * @param sizeInBytes Size of the serialized data
     * @return true if deserialization succeeded, false on error
     */
    static bool loadState(PatternModel& model, const void* data, int sizeInBytes);
    
private:
    // Magic number for file format validation ("SQBE" = SquareBeats)
    static constexpr uint32_t MAGIC_NUMBER = 0x53514245;
    
    // Version number for format compatibility
    // Version 3: Per-color pitch sequencer loop length
    // Version 4: Scale configuration (root note and scale type)
    // Version 5: Scale sequencer configuration (enabled state and segments)
    static constexpr uint32_t VERSION = 5;
    
    JUCE_DECLARE_NON_COPYABLE(StateManager)
};

} // namespace SquareBeats

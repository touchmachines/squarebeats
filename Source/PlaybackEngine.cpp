#include "PlaybackEngine.h"
#include "ConversionUtils.h"
#include "VisualFeedback.h"

namespace SquareBeats {

//==============================================================================
PlaybackEngine::PlaybackEngine()
    : pattern(nullptr)
    , currentPositionBeats(0.0)
    , absolutePositionBeats(0.0)
    , loopLengthBeats(0.0)
    , isPlaying(false)
    , sampleRate(44100.0)
    , bpm(120.0)
    , currentStepIndex(0)
    , totalSteps(16)
    , pendulumForward(true)
{
}

//==============================================================================
void PlaybackEngine::setPatternModel(PatternModel* model)
{
    pattern = model;
    
    // Recalculate loop length when pattern changes
    if (pattern != nullptr) {
        TimeSignature timeSig = pattern->getTimeSignature();
        double loopBars = pattern->getLoopLength();
        loopLengthBeats = loopBars * timeSig.getBeatsPerBar();
        totalSteps = calculateTotalSteps();
    }
}

//==============================================================================
void PlaybackEngine::handleTransportChange(bool playing, double sr, double tempo, 
                                          double timeInSamples, double timeInBeats)
{
    // Validate and clamp input parameters
    // Handle invalid sample rate (must be positive)
    if (sr <= 0.0 || sr > 1000000.0) {  // Sanity check: max 1MHz sample rate
        sr = 44100.0;  // Default to 44.1kHz
    }
    
    // Handle invalid tempo (must be positive and reasonable)
    if (tempo <= 0.0 || tempo > 999.0) {  // Sanity check: max 999 BPM
        tempo = 120.0;  // Default to 120 BPM
    }
    
    // Handle play/stop state changes
    bool wasPlaying = isPlaying;
    isPlaying = playing;
    
    // Update transport parameters
    sampleRate = sr;
    bpm = tempo;
    
    // If transport stopped, send note-offs for all active notes
    if (wasPlaying && !isPlaying) {
        juce::MidiBuffer tempBuffer;
        stopAllNotes(tempBuffer);
        activeNotesByColor.clear();
        
        // Clear all visual feedback states
        if (visualFeedback != nullptr) {
            visualFeedback->clearAllGates();
        }
        
        // Reset step position and pendulum direction
        currentStepIndex = 0;
        pendulumForward = true;
    }
    
    // Update absolute position for pitch sequencer (always tracks host)
    absolutePositionBeats = timeInBeats;
    if (absolutePositionBeats < 0.0) {
        absolutePositionBeats = 0.0;
    }
    
    // Only sync currentPositionBeats with host when transport JUST started
    // During playback, let updatePlaybackPosition handle position based on play mode
    bool transportJustStarted = !wasPlaying && isPlaying;
    
    if (transportJustStarted && pattern != nullptr && loopLengthBeats > 0.0) {
        double hostPosition = std::fmod(timeInBeats, loopLengthBeats);
        if (hostPosition < 0.0) {
            hostPosition += loopLengthBeats;
        }
        
        // For reverse mode, start at the end of the loop
        const PlayModeConfig& playModeConfig = pattern->getPlayModeConfig();
        if (playModeConfig.mode == PLAY_BACKWARD) {
            // Start from end of loop minus the host's offset
            currentPositionBeats = loopLengthBeats - hostPosition;
            if (currentPositionBeats < 0.0) currentPositionBeats = 0.0;
        } else {
            currentPositionBeats = hostPosition;
        }
        
        pendulumForward = true;
    }
}

//==============================================================================
void PlaybackEngine::updatePlaybackPosition(int numSamples)
{
    if (!isPlaying || pattern == nullptr || sampleRate <= 0.0 || bpm <= 0.0) {
        return;
    }
    
    // Calculate time advance in beats
    // beats = (samples / sampleRate) * (bpm / 60)
    double secondsElapsed = numSamples / sampleRate;
    double beatsElapsed = secondsElapsed * (bpm / 60.0);
    
    // Advance absolute position (for pitch sequencer - never wraps to main loop)
    absolutePositionBeats += beatsElapsed;
    
    // Get play mode configuration
    const PlayModeConfig& playModeConfig = pattern->getPlayModeConfig();
    
    // Calculate step duration
    totalSteps = calculateTotalSteps();
    double beatsPerStep = loopLengthBeats / totalSteps;
    
    // Track previous step to detect step changes
    int previousStep = currentStepIndex;
    
    // Advance position based on play mode
    switch (playModeConfig.mode) {
        case PLAY_FORWARD:
        default:
            currentPositionBeats += beatsElapsed;
            if (loopLengthBeats > 0.0 && currentPositionBeats >= loopLengthBeats) {
                currentPositionBeats = std::fmod(currentPositionBeats, loopLengthBeats);
            }
            currentStepIndex = static_cast<int>(currentPositionBeats / beatsPerStep) % totalSteps;
            break;
            
        case PLAY_BACKWARD:
            currentPositionBeats -= beatsElapsed;
            if (currentPositionBeats < 0.0) {
                currentPositionBeats = loopLengthBeats + std::fmod(currentPositionBeats, loopLengthBeats);
            }
            currentStepIndex = static_cast<int>(currentPositionBeats / beatsPerStep) % totalSteps;
            break;
            
        case PLAY_PENDULUM:
            if (pendulumForward) {
                currentPositionBeats += beatsElapsed;
                if (currentPositionBeats >= loopLengthBeats) {
                    currentPositionBeats = loopLengthBeats - (currentPositionBeats - loopLengthBeats);
                    pendulumForward = false;
                }
            } else {
                currentPositionBeats -= beatsElapsed;
                if (currentPositionBeats <= 0.0) {
                    currentPositionBeats = -currentPositionBeats;
                    pendulumForward = true;
                }
            }
            currentStepIndex = static_cast<int>(currentPositionBeats / beatsPerStep) % totalSteps;
            break;
            
        case PLAY_PROBABILITY:
            // In probability mode, we advance normally but may jump on step boundaries
            currentPositionBeats += beatsElapsed;
            if (loopLengthBeats > 0.0 && currentPositionBeats >= loopLengthBeats) {
                currentPositionBeats = std::fmod(currentPositionBeats, loopLengthBeats);
            }
            
            int newStep = static_cast<int>(currentPositionBeats / beatsPerStep) % totalSteps;
            
            // Check if we crossed a step boundary
            if (newStep != previousStep) {
                // Roll for probability jump
                if (randomGenerator.nextFloat() < playModeConfig.probability) {
                    // Jump by step jump size
                    int jumpSteps = playModeConfig.getStepJumpSteps();
                    
                    // Randomly choose direction
                    if (randomGenerator.nextBool()) {
                        currentStepIndex = (currentStepIndex + jumpSteps) % totalSteps;
                    } else {
                        currentStepIndex = (currentStepIndex - jumpSteps + totalSteps) % totalSteps;
                    }
                    
                    // Update position to match new step
                    currentPositionBeats = currentStepIndex * beatsPerStep;
                } else {
                    currentStepIndex = newStep;
                }
            }
            break;
    }
    
    // Ensure position stays in valid range
    if (currentPositionBeats < 0.0) {
        currentPositionBeats = 0.0;
    }
    if (loopLengthBeats > 0.0 && currentPositionBeats >= loopLengthBeats) {
        currentPositionBeats = std::fmod(currentPositionBeats, loopLengthBeats);
    }
}

//==============================================================================
float PlaybackEngine::getNormalizedPlaybackPosition() const
{
    if (loopLengthBeats <= 0.0) {
        return 0.0f;
    }
    return static_cast<float>(currentPositionBeats / loopLengthBeats);
}

float PlaybackEngine::getNormalizedPitchSeqPosition(int colorId) const
{
    if (pattern == nullptr) {
        return 0.0f;
    }
    
    const ColorChannelConfig& config = pattern->getColorConfig(colorId);
    TimeSignature timeSig = pattern->getTimeSignature();
    double pitchSeqLoopBeats = config.pitchSeqLoopLengthBars * timeSig.getBeatsPerBar();
    
    if (pitchSeqLoopBeats <= 0.0) {
        return 0.0f;
    }
    
    // Use absolute position so pitch sequencer runs independently of main loop
    double normalizedPos = std::fmod(absolutePositionBeats, pitchSeqLoopBeats) / pitchSeqLoopBeats;
    return static_cast<float>(normalizedPos);
}

float PlaybackEngine::getNormalizedScaleSeqPosition() const
{
    if (pattern == nullptr) {
        return 0.0f;
    }
    
    const ScaleSequencerConfig& scaleSeq = pattern->getScaleSequencer();
    if (!scaleSeq.enabled || scaleSeq.segments.empty()) {
        return 0.0f;
    }
    
    TimeSignature timeSig = pattern->getTimeSignature();
    double beatsPerBar = timeSig.getBeatsPerBar();
    int totalBars = scaleSeq.getTotalLengthBars();
    
    if (totalBars <= 0 || beatsPerBar <= 0.0) {
        return 0.0f;
    }
    
    double totalBeats = totalBars * beatsPerBar;
    double posInSequence = std::fmod(absolutePositionBeats, totalBeats);
    
    return static_cast<float>(posInSequence / totalBeats);
}

double PlaybackEngine::getPositionInBars() const
{
    if (pattern == nullptr) {
        return 0.0;
    }
    
    TimeSignature timeSig = pattern->getTimeSignature();
    double beatsPerBar = timeSig.getBeatsPerBar();
    
    if (beatsPerBar <= 0.0) {
        return 0.0;
    }
    
    return absolutePositionBeats / beatsPerBar;
}

void PlaybackEngine::resetPlaybackPosition()
{
    currentPositionBeats = 0.0;
    absolutePositionBeats = 0.0;
    currentStepIndex = 0;
    
    // Stop all active notes when resetting
    juce::MidiBuffer dummyBuffer;
    stopAllNotes(dummyBuffer);
}

void PlaybackEngine::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!isPlaying || pattern == nullptr) {
        return;
    }
    
    // Recalculate loop length in case it changed
    TimeSignature timeSig = pattern->getTimeSignature();
    double loopBars = pattern->getLoopLength();
    loopLengthBeats = loopBars * timeSig.getBeatsPerBar();
    
    int numSamples = buffer.getNumSamples();
    
    // Store position BEFORE updating (this is where we start this block)
    double blockStartBeats = currentPositionBeats;
    
    // Update playback position based on play mode
    // This advances currentPositionBeats according to the selected play mode
    updatePlaybackPosition(numSamples);
    
    // Now currentPositionBeats is where we END this block
    double blockEndBeats = currentPositionBeats;
    
    // Get play mode to determine how to process triggers
    const PlayModeConfig& playModeConfig = pattern->getPlayModeConfig();
    
    // Process square triggers based on play mode
    switch (playModeConfig.mode) {
        case PLAY_FORWARD:
        default:
            // Forward: process from start to end
            if (blockEndBeats < blockStartBeats) {
                // Wrapped around loop boundary
                processSquareTriggers(midiMessages, blockStartBeats, loopLengthBeats);
                processSquareTriggers(midiMessages, 0.0, blockEndBeats);
            } else {
                processSquareTriggers(midiMessages, blockStartBeats, blockEndBeats);
            }
            break;
            
        case PLAY_BACKWARD:
            // Backward: process from end to start (swap order)
            if (blockEndBeats > blockStartBeats) {
                // Wrapped around loop boundary (going backwards from near 0 to near end)
                processSquareTriggers(midiMessages, 0.0, blockStartBeats);
                processSquareTriggers(midiMessages, blockEndBeats, loopLengthBeats);
            } else {
                processSquareTriggers(midiMessages, blockEndBeats, blockStartBeats);
            }
            break;
            
        case PLAY_PENDULUM:
            // Pendulum: direction can change mid-block, process the range we covered
            {
                double minPos = std::min(blockStartBeats, blockEndBeats);
                double maxPos = std::max(blockStartBeats, blockEndBeats);
                processSquareTriggers(midiMessages, minPos, maxPos);
            }
            break;
            
        case PLAY_PROBABILITY:
            // Probability: may have jumped, process around current position
            // Use a small window around the current step
            {
                double beatsPerStep = loopLengthBeats / std::max(1, totalSteps);
                double stepStart = currentStepIndex * beatsPerStep;
                double stepEnd = stepStart + beatsPerStep;
                
                // Only trigger if we just entered this step (compare with previous position)
                if (blockStartBeats < stepStart || blockStartBeats >= stepEnd) {
                    // We entered a new step this block
                    processSquareTriggers(midiMessages, stepStart, stepEnd);
                }
            }
            break;
    }
}

//==============================================================================
void PlaybackEngine::processSquareTriggers(juce::MidiBuffer& midiMessages, 
                                          double startBeats, double endBeats)
{
    if (pattern == nullptr) {
        return;
    }
    
    // Validate time signature to prevent division by zero
    TimeSignature timeSig = pattern->getTimeSignature();
    if (timeSig.numerator <= 0 || timeSig.denominator <= 0) {
        timeSig = TimeSignature(4, 4);  // Default to 4/4 if invalid
    }
    
    double loopBars = pattern->getLoopLength();
    if (loopBars <= 0.0) {
        return;  // Can't process with invalid loop length
    }
    
    // Calculate the maximum quantization interval across all color channels
    // This determines how much we need to expand our search range
    double maxQuantizeInterval = 0.0;
    double beatsPerBar = timeSig.getBeatsPerBar();
    for (int colorId = 0; colorId < 4; ++colorId) {
        const ColorChannelConfig& config = pattern->getColorConfig(colorId);
        double quantizeInterval;
        switch (config.quantize) {
            case Q_1_32: quantizeInterval = beatsPerBar / 32.0; break;
            case Q_1_16: quantizeInterval = beatsPerBar / 16.0; break;
            case Q_1_8:  quantizeInterval = beatsPerBar / 8.0;  break;
            case Q_1_4:  quantizeInterval = beatsPerBar / 4.0;  break;
            case Q_1_2:  quantizeInterval = beatsPerBar / 2.0;  break;
            case Q_1_BAR: quantizeInterval = beatsPerBar;       break;
            default:     quantizeInterval = beatsPerBar / 16.0; break;
        }
        maxQuantizeInterval = std::max(maxQuantizeInterval, quantizeInterval);
    }
    
    // Expand the search range by the max quantization interval in both directions
    // This ensures we find squares whose raw position is outside the block
    // but whose quantized position falls within the block
    double expandedStartBeats = startBeats - maxQuantizeInterval;
    double expandedEndBeats = endBeats + maxQuantizeInterval;
    
    // Clamp expanded range to loop boundaries (will handle wrap-around separately)
    if (expandedStartBeats < 0.0) {
        expandedStartBeats = 0.0;
    }
    
    // Convert beat times to normalized coordinates
    float startNormalized = beatsToNormalized(expandedStartBeats, loopBars, timeSig);
    float endNormalized = beatsToNormalized(expandedEndBeats, loopBars, timeSig);
    
    // Handle loop wrap-around (squares spanning loop boundaries)
    bool wrapsAroundLoop = endNormalized < startNormalized || endBeats > loopLengthBeats;
    
    std::vector<Square*> squares;
    if (wrapsAroundLoop) {
        // Query two ranges: [start, 1.0] and [0.0, end]
        auto squares1 = pattern->getSquaresInTimeRange(startNormalized, 1.0f);
        auto squares2 = pattern->getSquaresInTimeRange(0.0f, endNormalized);
        squares.insert(squares.end(), squares1.begin(), squares1.end());
        squares.insert(squares.end(), squares2.begin(), squares2.end());
    } else {
        squares = pattern->getSquaresInTimeRange(startNormalized, endNormalized);
    }
    
    // Sort squares by gate time to handle multiple squares on same quantized time consistently
    // Process in order of creation (stable sort preserves original order for equal elements)
    std::stable_sort(squares.begin(), squares.end(), 
        [loopBars, &timeSig](const Square* a, const Square* b) {
            double gateA = normalizedToBeats(a->leftEdge, loopBars, timeSig);
            double gateB = normalizedToBeats(b->leftEdge, loopBars, timeSig);
            return gateA < gateB;
        });
    
    // Process each square
    for (Square* square : squares) {
        if (square == nullptr) continue;
        
        int colorId = square->colorChannelId;
        const ColorChannelConfig& config = pattern->getColorConfig(colorId);
        
        // Calculate gate time (left edge of square)
        double gateTimeBeats = normalizedToBeats(square->leftEdge, loopBars, timeSig);
        
        // Apply quantization
        double quantizedGateBeats = MIDIGenerator::applyQuantization(gateTimeBeats, config.quantize, timeSig);
        
        // Wrap quantized gate time to loop length
        if (quantizedGateBeats >= loopLengthBeats) {
            quantizedGateBeats = std::fmod(quantizedGateBeats, loopLengthBeats);
        }
        
        // Check if this trigger falls within current block
        bool triggerInBlock = false;
        if (wrapsAroundLoop) {
            // Handle wrap-around case: trigger is in block if it's after start OR before end
            double wrappedStart = std::fmod(startBeats, loopLengthBeats);
            double wrappedEnd = std::fmod(endBeats, loopLengthBeats);
            
            if (wrappedEnd < wrappedStart) {
                // Block wraps around loop boundary
                triggerInBlock = (quantizedGateBeats >= wrappedStart) || (quantizedGateBeats < wrappedEnd);
            } else {
                triggerInBlock = (quantizedGateBeats >= wrappedStart && quantizedGateBeats < wrappedEnd);
            }
        } else {
            triggerInBlock = (quantizedGateBeats >= startBeats && quantizedGateBeats < endBeats);
        }
        
        if (triggerInBlock) {
            // Calculate sample offset for this trigger
            double blockDurationBeats = endBeats - startBeats;
            int blockSamples = static_cast<int>(blockDurationBeats * 60.0 / bpm * sampleRate);
            int sampleOffset = calculateSampleOffset(quantizedGateBeats, startBeats, blockSamples);
            
            // If a note is already playing for this color, send note-off first (monophonic)
            if (activeNotesByColor.find(colorId) != activeNotesByColor.end()) {
                sendNoteOff(midiMessages, colorId, sampleOffset);
            }
            
            // Send new note-on
            sendNoteOn(midiMessages, *square, sampleOffset);
            
            // Track active note with end time
            double endTimeBeats = normalizedToBeats(square->getRightEdge(), loopBars, timeSig);
            
            // Wrap end time to loop length if square spans loop boundary
            if (endTimeBeats > loopLengthBeats) {
                endTimeBeats = std::fmod(endTimeBeats, loopLengthBeats);
            }
            
            // Calculate pitch offset for tracking (same logic as in sendNoteOn)
            // Pitch modulation is always applied regardless of editing mode
            float pitchOffset = 0.0f;
            if (!config.pitchWaveform.empty()) {
                double pitchSeqLoopBeats = config.pitchSeqLoopLengthBars * timeSig.getBeatsPerBar();
                if (pitchSeqLoopBeats > 0.0) {
                    // Use absolute position so pitch sequencer runs independently of main loop
                    double normalizedPitchSeqPos = std::fmod(absolutePositionBeats, pitchSeqLoopBeats) / pitchSeqLoopBeats;
                    pitchOffset = config.getPitchOffsetAt(normalizedPitchSeqPos);
                }
            }
            
            int midiNote = MIDIGenerator::calculateMidiNote(*square, config, pitchOffset, pattern->getActiveScale(getPositionInBars()));
            activeNotesByColor[colorId] = {midiNote, colorId, endTimeBeats};
        }
        
        // Check if note should end in this block
        auto activeIt = activeNotesByColor.find(colorId);
        if (activeIt != activeNotesByColor.end()) {
            double noteEndBeats = activeIt->second.endTime;
            
            bool noteEndsInBlock = false;
            if (wrapsAroundLoop) {
                double wrappedStart = std::fmod(startBeats, loopLengthBeats);
                double wrappedEnd = std::fmod(endBeats, loopLengthBeats);
                
                if (wrappedEnd < wrappedStart) {
                    noteEndsInBlock = (noteEndBeats >= wrappedStart) || (noteEndBeats < wrappedEnd);
                } else {
                    noteEndsInBlock = (noteEndBeats >= wrappedStart && noteEndBeats < wrappedEnd);
                }
            } else {
                noteEndsInBlock = (noteEndBeats >= startBeats && noteEndBeats < endBeats);
            }
            
            if (noteEndsInBlock) {
                double blockDurationBeats = endBeats - startBeats;
                int blockSamples = static_cast<int>(blockDurationBeats * 60.0 / bpm * sampleRate);
                int sampleOffset = calculateSampleOffset(noteEndBeats, startBeats, blockSamples);
                sendNoteOff(midiMessages, colorId, sampleOffset);
            }
        }
    }
}

//==============================================================================
void PlaybackEngine::sendNoteOff(juce::MidiBuffer& midiMessages, int colorId, int sampleOffset)
{
    auto it = activeNotesByColor.find(colorId);
    if (it == activeNotesByColor.end()) {
        return;
    }
    
    const ActiveNote& activeNote = it->second;
    const ColorChannelConfig& config = pattern->getColorConfig(colorId);
    
    juce::MidiMessage noteOff = MIDIGenerator::createNoteOff(config.midiChannel, activeNote.midiNote);
    midiMessages.addEvent(noteOff, sampleOffset);
    
    // Trigger visual feedback for gate-off
    if (visualFeedback != nullptr) {
        visualFeedback->triggerGateOff(colorId);
    }
    
    // Remove from active notes
    activeNotesByColor.erase(it);
}

//==============================================================================
void PlaybackEngine::sendNoteOn(juce::MidiBuffer& midiMessages, const Square& square, int sampleOffset)
{
    if (pattern == nullptr) {
        return;
    }
    
    int colorId = square.colorChannelId;
    const ColorChannelConfig& config = pattern->getColorConfig(colorId);
    
    // Get pitch offset from the color's pitch waveform
    // Pitch modulation is always applied regardless of editing mode
    float pitchOffset = 0.0f;
    
    if (!config.pitchWaveform.empty()) {
        // Use absolute position so pitch sequencer runs independently of main loop
        TimeSignature timeSig = pattern->getTimeSignature();
        double pitchSeqLoopBeats = config.pitchSeqLoopLengthBars * timeSig.getBeatsPerBar();
        
        // Validate pitch sequencer loop length
        if (pitchSeqLoopBeats > 0.0) {
            // Normalize absolute position to pitch sequencer's loop
            double normalizedPitchSeqPos = std::fmod(absolutePositionBeats, pitchSeqLoopBeats) / pitchSeqLoopBeats;
            
            pitchOffset = config.getPitchOffsetAt(normalizedPitchSeqPos);
        }
    }
    
    // Calculate MIDI note and velocity
    int midiNote = MIDIGenerator::calculateMidiNote(square, config, pitchOffset, pattern->getActiveScale(getPositionInBars()));
    int velocity = MIDIGenerator::calculateVelocity(square);
    
    // Create and add note-on message
    juce::MidiMessage noteOn = MIDIGenerator::createNoteOn(config.midiChannel, midiNote, velocity);
    midiMessages.addEvent(noteOn, sampleOffset);
    
    // Trigger visual feedback for gate-on
    if (visualFeedback != nullptr) {
        visualFeedback->triggerGateOn(colorId, velocity, square.uniqueId);
    }
}

//==============================================================================
int PlaybackEngine::calculateSampleOffset(double timeBeats, double blockStartBeats, int numSamples) const
{
    // Calculate how far into the block this event occurs
    double beatOffset = timeBeats - blockStartBeats;
    
    // Handle negative offsets (shouldn't happen, but clamp for safety)
    if (beatOffset < 0.0) {
        beatOffset = 0.0;
    }
    
    // Convert beats to samples
    double secondsOffset = beatOffset * 60.0 / bpm;
    int sampleOffset = static_cast<int>(secondsOffset * sampleRate);
    
    // Clamp to buffer size
    return std::clamp(sampleOffset, 0, numSamples - 1);
}

//==============================================================================
void PlaybackEngine::stopAllNotes(juce::MidiBuffer& midiMessages)
{
    for (const auto& pair : activeNotesByColor) {
        const ActiveNote& activeNote = pair.second;
        const ColorChannelConfig& config = pattern->getColorConfig(activeNote.colorChannelId);
        
        juce::MidiMessage noteOff = MIDIGenerator::createNoteOff(config.midiChannel, activeNote.midiNote);
        midiMessages.addEvent(noteOff, 0);
    }
    
    activeNotesByColor.clear();
}

//==============================================================================
int PlaybackEngine::calculateTotalSteps() const
{
    if (pattern == nullptr || loopLengthBeats <= 0.0) {
        return 16;  // Default
    }
    
    // Use 1/16 notes as the step grid for probability mode
    // This gives a musical grid that makes jumps noticeable
    // For a 4/4 bar, this gives 16 steps per bar
    TimeSignature timeSig = pattern->getTimeSignature();
    double beatsPerBar = timeSig.getBeatsPerBar();
    
    // 1/16 note = 1/4 of a beat in 4/4 time
    double stepInterval = beatsPerBar / 16.0;
    
    // Calculate total steps
    int steps = static_cast<int>(loopLengthBeats / stepInterval);
    return std::max(1, steps);
}

int PlaybackEngine::calculateNextStep()
{
    // This method is used for step-based advancement
    // Currently handled inline in updatePlaybackPosition
    return (currentStepIndex + 1) % totalSteps;
}

} // namespace SquareBeats

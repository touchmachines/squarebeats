#pragma once

#include <juce_graphics/juce_graphics.h>
#include <array>
#include <atomic>
#include <cmath>

namespace SquareBeats {

//==============================================================================
/**
 * Thread-safe gate event tracking for visual feedback
 * 
 * The audio thread sets gate events, and the UI thread reads them
 * to create visual effects like background flashes and activity indicators.
 */
struct GateEvent {
    std::atomic<bool> gateOn{false};      // True when note is currently playing
    std::atomic<float> triggerTime{0.0f}; // Time when last triggered (for flash decay)
    std::atomic<int> velocity{0};         // Velocity of the triggered note (0-127)
    std::atomic<int> activeSquareId{-1};  // UniqueId of the currently playing square (-1 = none)
    
    GateEvent() = default;
    
    // Non-copyable due to atomics
    GateEvent(const GateEvent&) = delete;
    GateEvent& operator=(const GateEvent&) = delete;
    
    // Move constructor for array initialization
    GateEvent(GateEvent&&) noexcept {}
};

//==============================================================================
/**
 * Visual feedback state for all 4 color channels
 * 
 * This class provides thread-safe communication between the audio thread
 * (which generates MIDI events) and the UI thread (which renders visual feedback).
 */
class VisualFeedbackState {
public:
    static constexpr int NUM_COLORS = 4;
    static constexpr float FLASH_DURATION_MS = 150.0f;  // How long the flash lasts
    static constexpr float GLOW_DURATION_MS = 100.0f;   // How long the active glow pulses
    
    VisualFeedbackState() {
        currentTimeMs.store(0.0f);
    }
    
    //==============================================================================
    // Called from audio thread
    
    /**
     * Signal that a gate-on event occurred for a color channel
     * @param colorId Color channel ID (0-3)
     * @param vel Velocity of the note (0-127)
     * @param squareId UniqueId of the square being triggered (-1 for unknown)
     */
    void triggerGateOn(int colorId, int vel, int squareId = -1) {
        if (colorId >= 0 && colorId < NUM_COLORS) {
            gateEvents[colorId].gateOn.store(true, std::memory_order_release);
            gateEvents[colorId].triggerTime.store(currentTimeMs.load(std::memory_order_acquire), std::memory_order_release);
            gateEvents[colorId].velocity.store(vel, std::memory_order_release);
            gateEvents[colorId].activeSquareId.store(squareId, std::memory_order_release);
        }
    }
    
    /**
     * Signal that a gate-off event occurred for a color channel
     */
    void triggerGateOff(int colorId) {
        if (colorId >= 0 && colorId < NUM_COLORS) {
            gateEvents[colorId].gateOn.store(false, std::memory_order_release);
            gateEvents[colorId].activeSquareId.store(-1, std::memory_order_release);
        }
    }
    
    /**
     * Clear all gate states (call when transport stops)
     */
    void clearAllGates() {
        for (int i = 0; i < NUM_COLORS; ++i) {
            gateEvents[i].gateOn.store(false, std::memory_order_release);
            // Set trigger time far in the past to stop any ongoing flashes
            gateEvents[i].triggerTime.store(-10000.0f, std::memory_order_release);
            gateEvents[i].activeSquareId.store(-1, std::memory_order_release);
        }
    }
    
    //==============================================================================
    // Called from UI thread
    
    /**
     * Update the current time (call this from timer callback)
     */
    void updateTime(float timeMs) {
        currentTimeMs.store(timeMs, std::memory_order_release);
    }
    
    /**
     * Get the flash intensity for a color channel (0.0 to 1.0)
     * Decays over FLASH_DURATION_MS
     */
    float getFlashIntensity(int colorId) const {
        if (colorId < 0 || colorId >= NUM_COLORS) return 0.0f;
        
        float triggerTime = gateEvents[colorId].triggerTime.load(std::memory_order_acquire);
        float currentTime = currentTimeMs.load(std::memory_order_acquire);
        float elapsed = currentTime - triggerTime;
        
        if (elapsed < 0.0f || elapsed > FLASH_DURATION_MS) return 0.0f;
        
        // Exponential decay for natural-looking fade
        float normalizedTime = elapsed / FLASH_DURATION_MS;
        return std::exp(-3.0f * normalizedTime) * (1.0f - normalizedTime);
    }
    
    /**
     * Check if a gate is currently on for a color channel
     */
    bool isGateOn(int colorId) const {
        if (colorId < 0 || colorId >= NUM_COLORS) return false;
        return gateEvents[colorId].gateOn.load(std::memory_order_acquire);
    }
    
    /**
     * Get the velocity of the last triggered note (0-127)
     */
    int getVelocity(int colorId) const {
        if (colorId < 0 || colorId >= NUM_COLORS) return 0;
        return gateEvents[colorId].velocity.load(std::memory_order_acquire);
    }
    
    /**
     * Get the uniqueId of the currently active (playing) square for a color channel
     * @return Square uniqueId, or -1 if no square is active
     */
    int getActiveSquareId(int colorId) const {
        if (colorId < 0 || colorId >= NUM_COLORS) return -1;
        return gateEvents[colorId].activeSquareId.load(std::memory_order_acquire);
    }
    
    /**
     * Get the glow intensity for active notes (pulses while gate is on)
     */
    float getActiveGlowIntensity(int colorId) const {
        if (!isGateOn(colorId)) return 0.0f;
        
        float currentTime = currentTimeMs.load(std::memory_order_acquire);
        // Subtle pulsing effect while note is held
        float pulse = 0.7f + 0.3f * std::sin(currentTime * 0.01f);
        return pulse;
    }

private:
    std::array<GateEvent, NUM_COLORS> gateEvents;
    std::atomic<float> currentTimeMs{0.0f};
};

//==============================================================================
/**
 * Beat pulse state for grid breathing effect
 */
class BeatPulseState {
public:
    static constexpr float BEAT_PULSE_DURATION_MS = 150.0f;
    
    BeatPulseState() = default;
    
    /**
     * Signal a beat occurred (call from audio thread)
     */
    void triggerBeat(bool isDownbeat) {
        lastBeatTimeMs.store(currentTimeMs.load(std::memory_order_acquire), std::memory_order_release);
        wasDownbeat.store(isDownbeat, std::memory_order_release);
    }
    
    /**
     * Update current time (call from UI thread)
     */
    void updateTime(float timeMs) {
        currentTimeMs.store(timeMs, std::memory_order_release);
    }
    
    /**
     * Get the beat pulse intensity (0.0 to 1.0)
     */
    float getPulseIntensity() const {
        float beatTime = lastBeatTimeMs.load(std::memory_order_acquire);
        float currentTime = currentTimeMs.load(std::memory_order_acquire);
        float elapsed = currentTime - beatTime;
        
        if (elapsed < 0.0f || elapsed > BEAT_PULSE_DURATION_MS) return 0.0f;
        
        float intensity = wasDownbeat.load(std::memory_order_acquire) ? 1.0f : 0.5f;
        float normalizedTime = elapsed / BEAT_PULSE_DURATION_MS;
        return intensity * std::exp(-4.0f * normalizedTime);
    }

private:
    std::atomic<float> lastBeatTimeMs{-1000.0f};  // Start in the past
    std::atomic<float> currentTimeMs{0.0f};
    std::atomic<bool> wasDownbeat{false};
};

} // namespace SquareBeats

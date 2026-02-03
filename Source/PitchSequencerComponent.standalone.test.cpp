/**
 * Standalone test for PitchSequencerComponent
 * Tests core functionality without requiring full JUCE setup
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <cstdint>
#include <algorithm>

// Minimal mock of JUCE types needed for testing
namespace juce {
    struct Colour {
        uint32_t value;
        Colour(uint32_t v = 0) : value(v) {}
        static Colour white;
        static Colour cyan;
        Colour withAlpha(float) const { return *this; }
    };
    Colour Colour::white = Colour(0xffffffff);
    Colour Colour::cyan = Colour(0xff00ffff);
    
    struct Point {
        float x, y;
        Point(float x_ = 0, float y_ = 0) : x(x_), y(y_) {}
    };
    
    struct Rectangle {
        float x, y, width, height;
        Rectangle(float x_ = 0, float y_ = 0, float w_ = 0, float h_ = 0) 
            : x(x_), y(y_), width(w_), height(h_) {}
        float getX() const { return x; }
        float getY() const { return y; }
        float getWidth() const { return width; }
        float getHeight() const { return height; }
        float getCentreY() const { return y + height * 0.5f; }
        Rectangle toFloat() const { return *this; }
    };
    
    struct Graphics {
        void fillAll(Colour) {}
        void setColour(Colour) {}
        void drawLine(float, float, float, float, float) {}
        void fillEllipse(float, float, float, float) {}
        void strokePath(class Path&, class PathStrokeType&) {}
    };
    
    struct Path {
        void startNewSubPath(float, float) {}
        void lineTo(float, float) {}
    };
    
    struct PathStrokeType {
        PathStrokeType(float) {}
    };
    
    struct MouseEvent {
        Point position;
        struct Mods {
            bool isPopupMenu() const { return false; }
        } mods;
    };
    
    struct Component {
        bool visible = false;
        Rectangle bounds;
        
        void setVisible(bool v) { visible = v; }
        bool isVisible() const { return visible; }
        void setInterceptsMouseClicks(bool, bool) {}
        Rectangle getLocalBounds() const { return bounds; }
        void repaint() {}
    };
    
    template<typename T>
    T jlimit(T min, T val, T max) {
        return val < min ? min : (val > max ? max : val);
    }
}

// Define the data structures inline to avoid including JuceHeader.h
namespace SquareBeats {

struct TimeSignature {
    int numerator;
    int denominator;
    
    TimeSignature() : numerator(4), denominator(4) {}
    TimeSignature(int num, int denom) : numerator(num), denominator(denom) {}
    
    double getBeatsPerBar() const {
        return numerator * (4.0 / denominator);
    }
};

enum QuantizationValue {
    Q_1_32, Q_1_16, Q_1_8, Q_1_4, Q_1_2, Q_1_BAR
};

struct ColorChannelConfig {
    int midiChannel;
    int highNote;
    int lowNote;
    QuantizationValue quantize;
    juce::Colour displayColor;
    
    ColorChannelConfig()
        : midiChannel(1), highNote(84), lowNote(48)
        , quantize(Q_1_16), displayColor(juce::Colour(0xffff0000))
    {}
};

struct PitchSequencer {
    std::vector<float> waveform;
    int loopLengthBars;
    bool editingPitch;  // True when editing pitch sequence, false when editing squares
    
    PitchSequencer() : loopLengthBars(2), editingPitch(false) {}
    
    float getPitchOffsetAt(double normalizedPosition) const {
        if (waveform.empty()) {
            return 0.0f;
        }
        
        normalizedPosition = normalizedPosition - std::floor(normalizedPosition);
        double indexFloat = normalizedPosition * (waveform.size() - 1);
        int index0 = static_cast<int>(std::floor(indexFloat));
        int index1 = std::min(index0 + 1, static_cast<int>(waveform.size() - 1));
        
        float t = static_cast<float>(indexFloat - index0);
        return waveform[index0] * (1.0f - t) + waveform[index1] * t;
    }
};

struct Square {
    float leftEdge, width, topEdge, height;
    int colorChannelId;
    uint32_t uniqueId;
    
    Square() : leftEdge(0), width(0), topEdge(0), height(0), colorChannelId(0), uniqueId(0) {}
    Square(float l, float t, float w, float h, int c, uint32_t id)
        : leftEdge(l), width(w), topEdge(t), height(h), colorChannelId(c), uniqueId(id) {}
};

} // namespace SquareBeats

// Mock PatternModel for testing
namespace SquareBeats {

class PatternModel {
public:
    PatternModel() {}
    
    PitchSequencer& getPitchSequencer() { return pitchSequencer; }
    const PitchSequencer& getPitchSequencer() const { return pitchSequencer; }
    
private:
    PitchSequencer pitchSequencer;
};

} // namespace SquareBeats

using namespace SquareBeats;

// Mock PitchSequencerComponent for testing
class PitchSequencerComponentTest : public juce::Component {
public:
    PitchSequencerComponentTest(PatternModel& model)
        : patternModel(model)
        , waveformResolution(256)
        , isDrawing(false)
    {
        auto& pitchSequencer = patternModel.getPitchSequencer();
        if (pitchSequencer.waveform.empty())
        {
            initializeWaveform();
        }
        updateVisibility();
    }
    
    void updateVisibility() {
        auto& pitchSequencer = patternModel.getPitchSequencer();
        // Component is always visible (waveform always shows)
        setVisible(true);
        // Mouse interception depends on editing mode
        setInterceptsMouseClicks(pitchSequencer.editingPitch, false);
    }
    
    void setWaveformResolution(int numSamples) {
        if (numSamples > 0 && numSamples != waveformResolution)
        {
            waveformResolution = numSamples;
            initializeWaveform();
        }
    }
    
    int getWaveformResolution() const { return waveformResolution; }
    
    float pixelXToNormalized(float pixelX) const {
        auto bounds = getLocalBounds();
        return (pixelX - bounds.getX()) / bounds.getWidth();
    }
    
    float pixelYToPitchOffset(float pixelY) const {
        auto bounds = getLocalBounds();
        float centerY = bounds.getCentreY();
        float normalizedY = (pixelY - centerY) / (bounds.getHeight() * 0.5f);
        return -normalizedY * 12.0f;
    }
    
    float pitchOffsetToPixelY(float pitchOffset) const {
        auto bounds = getLocalBounds();
        float centerY = bounds.getCentreY();
        float normalizedY = -pitchOffset / 12.0f;
        return centerY + normalizedY * (bounds.getHeight() * 0.5f);
    }
    
    void recordPitchOffset(float normalizedX, float pitchOffset) {
        auto& pitchSequencer = patternModel.getPitchSequencer();
        
        if (pitchSequencer.waveform.empty())
        {
            initializeWaveform();
        }
        
        int index = static_cast<int>(normalizedX * (pitchSequencer.waveform.size() - 1));
        index = juce::jlimit(0, static_cast<int>(pitchSequencer.waveform.size() - 1), index);
        
        pitchSequencer.waveform[index] = pitchOffset;
    }
    
    void simulateMouseDown(float x, float y) {
        auto& pitchSequencer = patternModel.getPitchSequencer();
        if (!pitchSequencer.editingPitch) return;
        
        isDrawing = true;
        float normalizedX = juce::jlimit(0.0f, 1.0f, pixelXToNormalized(x));
        float pitchOffset = juce::jlimit(-12.0f, 12.0f, pixelYToPitchOffset(y));
        recordPitchOffset(normalizedX, pitchOffset);
    }
    
    void simulateMouseDrag(float x, float y) {
        if (!isDrawing) return;
        
        float normalizedX = juce::jlimit(0.0f, 1.0f, pixelXToNormalized(x));
        float pitchOffset = juce::jlimit(-12.0f, 12.0f, pixelYToPitchOffset(y));
        recordPitchOffset(normalizedX, pitchOffset);
    }
    
    void simulateMouseUp() {
        isDrawing = false;
    }

private:
    PatternModel& patternModel;
    int waveformResolution;
    bool isDrawing;
    
    void initializeWaveform() {
        auto& pitchSequencer = patternModel.getPitchSequencer();
        pitchSequencer.waveform.clear();
        pitchSequencer.waveform.resize(waveformResolution, 0.0f);
    }
};

// Test functions
void testInitialization()
{
    std::cout << "Testing PitchSequencerComponent initialization..." << std::endl;
    
    PatternModel model;
    PitchSequencerComponentTest component(model);
    
    // Test initial state - should not be in editing mode by default
    auto& pitchSequencer = model.getPitchSequencer();
    assert(!pitchSequencer.editingPitch);
    
    // Component is always visible now (waveform always shows)
    assert(component.isVisible());
    
    // Test waveform is initialized
    assert(!pitchSequencer.waveform.empty());
    assert(pitchSequencer.waveform.size() == 256); // Default resolution
    
    // All values should be zero initially
    for (float value : pitchSequencer.waveform)
    {
        assert(value == 0.0f);
    }
    
    std::cout << "  ✓ Initialization tests passed" << std::endl;
}

void testVisibilityToggle()
{
    std::cout << "Testing editing mode toggle..." << std::endl;
    
    PatternModel model;
    PitchSequencerComponentTest component(model);
    auto& pitchSequencer = model.getPitchSequencer();
    
    // Initially not in editing mode
    assert(!pitchSequencer.editingPitch);
    // But component is always visible (waveform always shows)
    assert(component.isVisible());
    
    // Enable editing mode
    pitchSequencer.editingPitch = true;
    component.updateVisibility();
    assert(component.isVisible()); // Still visible
    
    // Disable editing mode
    pitchSequencer.editingPitch = false;
    component.updateVisibility();
    assert(component.isVisible()); // Still visible (waveform always shows)
    
    std::cout << "  ✓ Editing mode toggle tests passed" << std::endl;
}

void testWaveformResolution()
{
    std::cout << "Testing waveform resolution..." << std::endl;
    
    PatternModel model;
    PitchSequencerComponentTest component(model);
    auto& pitchSequencer = model.getPitchSequencer();
    
    // Test default resolution
    assert(component.getWaveformResolution() == 256);
    assert(pitchSequencer.waveform.size() == 256);
    
    // Change resolution
    component.setWaveformResolution(512);
    assert(component.getWaveformResolution() == 512);
    assert(pitchSequencer.waveform.size() == 512);
    
    // All values should be zero after resize
    for (float value : pitchSequencer.waveform)
    {
        assert(value == 0.0f);
    }
    
    // Test invalid resolution (should not change)
    component.setWaveformResolution(0);
    assert(component.getWaveformResolution() == 512);
    
    component.setWaveformResolution(-10);
    assert(component.getWaveformResolution() == 512);
    
    std::cout << "  ✓ Waveform resolution tests passed" << std::endl;
}

void testCoordinateConversion()
{
    std::cout << "Testing coordinate conversion..." << std::endl;
    
    PatternModel model;
    PitchSequencerComponentTest component(model);
    
    // Set component bounds for testing
    component.bounds = juce::Rectangle(0.0f, 0.0f, 800.0f, 400.0f);
    
    // Test X coordinate conversion
    assert(std::abs(component.pixelXToNormalized(0.0f) - 0.0f) < 0.001f);
    assert(std::abs(component.pixelXToNormalized(400.0f) - 0.5f) < 0.001f);
    assert(std::abs(component.pixelXToNormalized(800.0f) - 1.0f) < 0.001f);
    
    // Test Y coordinate to pitch offset conversion
    // Center (200) should be 0 semitones
    assert(std::abs(component.pixelYToPitchOffset(200.0f) - 0.0f) < 0.001f);
    
    // Top (0) should be +12 semitones
    assert(std::abs(component.pixelYToPitchOffset(0.0f) - 12.0f) < 0.001f);
    
    // Bottom (400) should be -12 semitones
    assert(std::abs(component.pixelYToPitchOffset(400.0f) - (-12.0f)) < 0.001f);
    
    // Test pitch offset to Y coordinate conversion (round-trip)
    float testOffsets[] = {-12.0f, -6.0f, 0.0f, 6.0f, 12.0f};
    for (float offset : testOffsets)
    {
        float pixelY = component.pitchOffsetToPixelY(offset);
        float convertedOffset = component.pixelYToPitchOffset(pixelY);
        assert(std::abs(convertedOffset - offset) < 0.001f);
    }
    
    std::cout << "  ✓ Coordinate conversion tests passed" << std::endl;
}

void testWaveformRecording()
{
    std::cout << "Testing waveform recording..." << std::endl;
    
    PatternModel model;
    PitchSequencerComponentTest component(model);
    auto& pitchSequencer = model.getPitchSequencer();
    
    // Set component bounds
    component.bounds = juce::Rectangle(0.0f, 0.0f, 800.0f, 400.0f);
    
    // Enable editing mode for recording
    pitchSequencer.editingPitch = true;
    component.updateVisibility();
    
    // Record some pitch offsets at specific positions
    component.simulateMouseDown(0.0f, 200.0f);  // Start at center (0 semitones)
    
    // Record multiple points along the waveform
    for (int i = 0; i <= 10; ++i)
    {
        float x = i * 80.0f;  // 0, 80, 160, ..., 800
        float y = 200.0f - i * 10.0f;  // Gradually move up (positive pitch)
        component.simulateMouseDrag(x, y);
    }
    
    component.simulateMouseUp();
    
    // Verify waveform was recorded
    // First sample should be near 0
    assert(std::abs(pitchSequencer.waveform[0] - 0.0f) < 1.0f);
    
    // Last sample should be positive (we moved up)
    assert(pitchSequencer.waveform.back() > 0.0f);
    
    std::cout << "  ✓ Waveform recording tests passed" << std::endl;
}

void testRecordingWhenHidden()
{
    std::cout << "Testing recording when not in editing mode..." << std::endl;
    
    PatternModel model;
    PitchSequencerComponentTest component(model);
    auto& pitchSequencer = model.getPitchSequencer();
    
    // Set component bounds
    component.bounds = juce::Rectangle(0.0f, 0.0f, 800.0f, 400.0f);
    
    // Keep not in editing mode (default state)
    assert(!pitchSequencer.editingPitch);
    
    // Try to record - should not work when not in editing mode
    component.simulateMouseDown(400.0f, 100.0f);
    component.simulateMouseUp();
    
    // Waveform should still be all zeros
    for (float value : pitchSequencer.waveform)
    {
        assert(value == 0.0f);
    }
    
    std::cout << "  ✓ Recording when not in editing mode tests passed" << std::endl;
}

void testPitchOffsetClamping()
{
    std::cout << "Testing pitch offset clamping..." << std::endl;
    
    PatternModel model;
    PitchSequencerComponentTest component(model);
    auto& pitchSequencer = model.getPitchSequencer();
    
    // Set component bounds
    component.bounds = juce::Rectangle(0.0f, 0.0f, 800.0f, 400.0f);
    
    // Enable editing mode
    pitchSequencer.editingPitch = true;
    component.updateVisibility();
    
    // Try to record values beyond the range
    // Y = -100 would be > +12 semitones, should clamp to +12
    component.simulateMouseDown(400.0f, -100.0f);
    component.simulateMouseUp();
    
    int midIndex = pitchSequencer.waveform.size() / 2;
    assert(pitchSequencer.waveform[midIndex] <= 12.0f);
    assert(pitchSequencer.waveform[midIndex] >= -12.0f);
    
    // Reset waveform
    component.setWaveformResolution(256);
    
    // Y = 500 would be < -12 semitones, should clamp to -12
    component.simulateMouseDown(400.0f, 500.0f);
    component.simulateMouseUp();
    
    assert(pitchSequencer.waveform[midIndex] <= 12.0f);
    assert(pitchSequencer.waveform[midIndex] >= -12.0f);
    
    std::cout << "  ✓ Pitch offset clamping tests passed" << std::endl;
}

int main()
{
    std::cout << "Running PitchSequencerComponent Tests..." << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try
    {
        testInitialization();
        testVisibilityToggle();
        testWaveformResolution();
        testCoordinateConversion();
        testWaveformRecording();
        testRecordingWhenHidden();
        testPitchOffsetClamping();
        
        std::cout << std::endl;
        std::cout << "All tests passed!" << std::endl;
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

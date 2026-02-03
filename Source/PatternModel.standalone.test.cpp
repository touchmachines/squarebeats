#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>

// Minimal JUCE replacements for standalone testing
namespace juce {
    template<typename T>
    T jlimit(T minValue, T maxValue, T value) {
        return (value < minValue) ? minValue : ((value > maxValue) ? maxValue : value);
    }
    
    class Colour {
    public:
        Colour(uint32_t rgba) : rgba_(rgba) {}
        uint32_t rgba_;
    };
    
    class ChangeBroadcaster {
    public:
        void sendChangeMessage() {}
    };
}

// Include data structures
enum QuantizationValue {
    Q_1_32,
    Q_1_16,
    Q_1_8,
    Q_1_4,
    Q_1_2,
    Q_1_BAR
};

struct TimeSignature {
    int numerator;
    int denominator;
    
    TimeSignature() : numerator(4), denominator(4) {}
    TimeSignature(int num, int denom) : numerator(num), denominator(denom) {}
};

struct Square {
    float leftEdge;
    float topEdge;
    float width;
    float height;
    int colorChannelId;
    uint32_t uniqueId;
    
    Square(float left, float top, float w, float h, int colorId, uint32_t id)
        : leftEdge(left), topEdge(top), width(w), height(h)
        , colorChannelId(colorId), uniqueId(id) {}
    
    float getRightEdge() const { return leftEdge + width; }
    float getBottomEdge() const { return topEdge + height; }
};

struct ColorChannelConfig {
    int midiChannel;
    int highNote;
    int lowNote;
    QuantizationValue quantize;
    juce::Colour displayColor;
    
    ColorChannelConfig()
        : midiChannel(1), highNote(84), lowNote(48)
        , quantize(Q_1_16), displayColor(0xFFFFFFFF) {}
};

struct PitchSequencer {
    std::vector<float> waveform;
    int loopLengthBars;
    bool visible;
    
    PitchSequencer() : loopLengthBars(2), visible(false) {}
    
    float getPitchOffsetAt(double normalizedPosition) const {
        if (!visible || waveform.empty()) {
            return 0.0f;
        }
        
        // Wrap position to [0, 1) range
        while (normalizedPosition >= 1.0) {
            normalizedPosition -= 1.0;
        }
        while (normalizedPosition < 0.0) {
            normalizedPosition += 1.0;
        }
        
        // Map normalized position to waveform index
        double indexFloat = normalizedPosition * (waveform.size() - 1);
        int index = static_cast<int>(indexFloat);
        double fraction = indexFloat - index;
        
        // Handle edge case: exactly at last sample
        if (index >= static_cast<int>(waveform.size()) - 1) {
            return waveform.back();
        }
        
        // Linear interpolation between samples
        float value1 = waveform[index];
        float value2 = waveform[index + 1];
        return value1 + (value2 - value1) * static_cast<float>(fraction);
    }
};

// Include PatternModel implementation inline
namespace SquareBeats {

class PatternModel : public juce::ChangeBroadcaster {
public:
    PatternModel() : loopLengthBars(2), timeSignature(4, 4), nextUniqueId(1) {
        initializeDefaultColorConfigs();
    }
    
    Square* createSquare(float left, float top, float width, float height, int colorId) {
        constexpr float MIN_SIZE = 0.01f;
        left = juce::jlimit(0.0f, 1.0f, left);
        top = juce::jlimit(0.0f, 1.0f, top);
        width = juce::jlimit(MIN_SIZE, 1.0f - left, width);
        height = juce::jlimit(MIN_SIZE, 1.0f - top, height);
        colorId = juce::jlimit(0, 3, colorId);
        
        uint32_t id = nextUniqueId++;
        squares.emplace_back(left, top, width, height, colorId, id);
        sendChangeMessage();
        return &squares.back();
    }
    
    bool deleteSquare(uint32_t squareId) {
        auto it = findSquareById(squareId);
        if (it != squares.end()) {
            squares.erase(it);
            sendChangeMessage();
            return true;
        }
        return false;
    }
    
    bool moveSquare(uint32_t squareId, float newLeft, float newTop) {
        auto it = findSquareById(squareId);
        if (it != squares.end()) {
            it->leftEdge = juce::jlimit(0.0f, 1.0f, newLeft);
            it->topEdge = juce::jlimit(0.0f, 1.0f, newTop);
            sendChangeMessage();
            return true;
        }
        return false;
    }
    
    bool resizeSquare(uint32_t squareId, float newWidth, float newHeight) {
        constexpr float MIN_SIZE = 0.01f;
        auto it = findSquareById(squareId);
        if (it != squares.end()) {
            it->width = juce::jlimit(MIN_SIZE, 1.0f - it->leftEdge, newWidth);
            it->height = juce::jlimit(MIN_SIZE, 1.0f - it->topEdge, newHeight);
            sendChangeMessage();
            return true;
        }
        return false;
    }
    
    void clearColorChannel(int colorId) {
        squares.erase(
            std::remove_if(squares.begin(), squares.end(),
                [colorId](const Square& square) {
                    return square.colorChannelId == colorId;
                }),
            squares.end()
        );
        sendChangeMessage();
    }
    
    std::vector<Square*> getSquaresInTimeRange(float startTime, float endTime) {
        std::vector<Square*> result;
        for (auto& square : squares) {
            float squareStart = square.leftEdge;
            float squareEnd = square.getRightEdge();
            if (squareStart < endTime && squareEnd > startTime) {
                result.push_back(&square);
            }
        }
        return result;
    }
    
    std::vector<Square*> getAllSquares() {
        std::vector<Square*> result;
        result.reserve(squares.size());
        for (auto& square : squares) {
            result.push_back(&square);
        }
        return result;
    }
    
    ColorChannelConfig& getColorConfig(int colorId) {
        colorId = juce::jlimit(0, 3, colorId);
        return colorConfigs[colorId];
    }
    
    void setColorConfig(int colorId, const ColorChannelConfig& config) {
        colorId = juce::jlimit(0, 3, colorId);
        colorConfigs[colorId] = config;
        sendChangeMessage();
    }
    
    PitchSequencer& getPitchSequencer() {
        return pitchSequencer;
    }
    
    void setLoopLength(int bars) {
        int oldLoopLength = loopLengthBars;
        
        if (bars <= 1)
            loopLengthBars = 1;
        else if (bars <= 2)
            loopLengthBars = 2;
        else
            loopLengthBars = 4;
        
        // If loop length decreased, remove squares that extend beyond the new length
        if (loopLengthBars < oldLoopLength) {
            float maxNormalizedPosition = static_cast<float>(loopLengthBars) / static_cast<float>(oldLoopLength);
            
            squares.erase(
                std::remove_if(squares.begin(), squares.end(),
                    [maxNormalizedPosition](const Square& square) {
                        return square.leftEdge >= maxNormalizedPosition;
                    }),
                squares.end()
            );
        }
        
        sendChangeMessage();
    }
    
    int getLoopLength() const {
        return loopLengthBars;
    }
    
    void setTimeSignature(int numerator, int denominator) {
        numerator = juce::jlimit(1, 16, numerator);
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
    
    TimeSignature getTimeSignature() const {
        return timeSignature;
    }
    
private:
    std::vector<Square> squares;
    std::array<ColorChannelConfig, 4> colorConfigs;
    PitchSequencer pitchSequencer;
    int loopLengthBars;
    TimeSignature timeSignature;
    uint32_t nextUniqueId;
    
    std::vector<Square>::iterator findSquareById(uint32_t squareId) {
        return std::find_if(squares.begin(), squares.end(),
            [squareId](const Square& square) {
                return square.uniqueId == squareId;
            });
    }
    
    void initializeDefaultColorConfigs() {
        colorConfigs[0].midiChannel = 1;
        colorConfigs[0].highNote = 84;
        colorConfigs[0].lowNote = 48;
        colorConfigs[0].quantize = Q_1_16;
        colorConfigs[0].displayColor = juce::Colour(0xFFFF4444);
        
        colorConfigs[1].midiChannel = 2;
        colorConfigs[1].highNote = 84;
        colorConfigs[1].lowNote = 48;
        colorConfigs[1].quantize = Q_1_16;
        colorConfigs[1].displayColor = juce::Colour(0xFF44FF44);
        
        colorConfigs[2].midiChannel = 3;
        colorConfigs[2].highNote = 84;
        colorConfigs[2].lowNote = 48;
        colorConfigs[2].quantize = Q_1_16;
        colorConfigs[2].displayColor = juce::Colour(0xFF4444FF);
        
        colorConfigs[3].midiChannel = 4;
        colorConfigs[3].highNote = 84;
        colorConfigs[3].lowNote = 48;
        colorConfigs[3].quantize = Q_1_16;
        colorConfigs[3].displayColor = juce::Colour(0xFFFFFF44);
    }
};

} // namespace SquareBeats

using namespace SquareBeats;

// Test functions
void testLoopLengthChangePreservesSquares()
{
    PatternModel model;
    
    // Start with 4 bar loop
    model.setLoopLength(4);
    
    // Create squares at different positions
    auto* sq1 = model.createSquare(0.1f, 0.5f, 0.1f, 0.1f, 0);
    auto* sq2 = model.createSquare(0.4f, 0.5f, 0.1f, 0.1f, 1);
    auto* sq3 = model.createSquare(0.7f, 0.5f, 0.1f, 0.1f, 2);
    
    uint32_t id1 = sq1->uniqueId;
    uint32_t id2 = sq2->uniqueId;
    uint32_t id3 = sq3->uniqueId;
    
    assert(model.getAllSquares().size() == 3);
    
    // Change to 2 bar loop (half the length)
    // Squares at normalized position >= 0.5 should be removed
    model.setLoopLength(2);
    
    auto squares = model.getAllSquares();
    assert(squares.size() == 2);
    
    // Verify the correct squares remain
    bool foundSq1 = false;
    bool foundSq2 = false;
    bool foundSq3 = false;
    
    for (auto* sq : squares)
    {
        if (sq->uniqueId == id1) foundSq1 = true;
        if (sq->uniqueId == id2) foundSq2 = true;
        if (sq->uniqueId == id3) foundSq3 = true;
    }
    
    assert(foundSq1);
    assert(foundSq2);
    assert(!foundSq3);
    
    std::cout << "✓ Loop length change preserves squares test passed\n";
}

void testLoopLengthIncreasePreservesAllSquares()
{
    PatternModel model;
    
    // Start with 1 bar loop
    model.setLoopLength(1);
    
    // Create squares
    model.createSquare(0.2f, 0.5f, 0.1f, 0.1f, 0);
    model.createSquare(0.5f, 0.5f, 0.1f, 0.1f, 1);
    model.createSquare(0.8f, 0.5f, 0.1f, 0.1f, 2);
    
    assert(model.getAllSquares().size() == 3);
    
    // Increase to 4 bars - all squares should be preserved
    model.setLoopLength(4);
    
    assert(model.getAllSquares().size() == 3);
    
    std::cout << "✓ Loop length increase preserves all squares test passed\n";
}

void testLoopLengthDecreaseBoundaryCase()
{
    PatternModel model;
    
    // Start with 4 bar loop
    model.setLoopLength(4);
    
    // Create squares exactly at boundaries
    auto* sq1 = model.createSquare(0.49f, 0.5f, 0.1f, 0.1f, 0);
    auto* sq2 = model.createSquare(0.50f, 0.5f, 0.1f, 0.1f, 1);
    
    uint32_t id1 = sq1->uniqueId;
    uint32_t id2 = sq2->uniqueId;
    
    // Change to 2 bar loop (threshold at 0.5)
    model.setLoopLength(2);
    
    auto squares = model.getAllSquares();
    
    // sq1 (at 0.49) should be preserved
    // sq2 (at 0.50) should be removed (>= threshold)
    assert(squares.size() == 1);
    assert(squares[0]->uniqueId == id1);
    
    std::cout << "✓ Loop length decrease boundary case test passed\n";
}

void testLoopLengthDecreaseFrom4To1()
{
    PatternModel model;
    
    // Start with 4 bar loop
    model.setLoopLength(4);
    
    // Create squares across the full range
    auto* sq1 = model.createSquare(0.1f, 0.5f, 0.1f, 0.1f, 0);   // Should remain (0.1 < 0.25)
    auto* sq2 = model.createSquare(0.24f, 0.5f, 0.1f, 0.1f, 1);  // Should remain (0.24 < 0.25)
    auto* sq3 = model.createSquare(0.25f, 0.5f, 0.1f, 0.1f, 2);  // Should be removed (0.25 >= 0.25)
    auto* sq4 = model.createSquare(0.5f, 0.5f, 0.1f, 0.1f, 3);   // Should be removed (0.5 >= 0.25)
    
    uint32_t id1 = sq1->uniqueId;
    uint32_t id2 = sq2->uniqueId;
    
    assert(model.getAllSquares().size() == 4);
    
    // Change to 1 bar loop (1/4 of original, threshold at 0.25)
    model.setLoopLength(1);
    
    auto squares = model.getAllSquares();
    assert(squares.size() == 2);
    
    // Verify only first two squares remain
    bool foundSq1 = false;
    bool foundSq2 = false;
    
    for (auto* sq : squares)
    {
        if (sq->uniqueId == id1) foundSq1 = true;
        if (sq->uniqueId == id2) foundSq2 = true;
    }
    
    assert(foundSq1);
    assert(foundSq2);
    
    std::cout << "✓ Loop length decrease from 4 to 1 test passed\n";
}

int main()
{
    std::cout << "Running PatternModel loop length change tests...\n\n";
    
    try
    {
        testLoopLengthChangePreservesSquares();
        testLoopLengthIncreasePreservesAllSquares();
        testLoopLengthDecreaseBoundaryCase();
        testLoopLengthDecreaseFrom4To1();
        
        std::cout << "\n✓ All loop length change tests passed!\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}

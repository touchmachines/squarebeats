#include "ScaleSequencerComponent.h"

namespace SquareBeats {

//==============================================================================
ScaleSequencerComponent::ScaleSequencerComponent(PatternModel& model)
    : patternModel(model)
{
    // Add button
    addButton.setButtonText("+");
    addButton.onClick = [this]() { onAddSegment(); };
    addAndMakeVisible(addButton);
    
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

ScaleSequencerComponent::~ScaleSequencerComponent()
{
    hideSegmentEditor();
}

//==============================================================================
void ScaleSequencerComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Background
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    // Title
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText("Scale Sequence", bounds.removeFromTop(25.0f), juce::Justification::centred);
    
    // Timeline area
    auto timelineArea = bounds.reduced(10.0f, 5.0f);
    timelineArea.removeFromRight(40.0f);  // Space for add button
    
    // Draw segments
    auto& config = patternModel.getScaleSequencer();
    
    if (config.segments.empty()) {
        g.setColour(juce::Colours::grey);
        g.drawText("No segments - click + to add", timelineArea, juce::Justification::centred);
        return;
    }
    
    // Calculate total bars for scaling
    int totalBars = config.getTotalLengthBars();
    if (totalBars <= 0) return;
    
    float pixelsPerBar = timelineArea.getWidth() / totalBars;
    float currentX = timelineArea.getX();
    
    for (size_t i = 0; i < config.segments.size(); ++i) {
        const auto& segment = config.segments[i];
        float segmentWidth = segment.lengthBars * pixelsPerBar;
        
        juce::Rectangle<float> segBounds(currentX, timelineArea.getY(), 
                                          segmentWidth, timelineArea.getHeight());
        
        // Segment background
        juce::Colour segColor = getSegmentColor(static_cast<int>(i));
        if (static_cast<int>(i) == hoveredSegment) {
            segColor = segColor.brighter(0.2f);
        }
        if (static_cast<int>(i) == selectedSegment) {
            segColor = segColor.brighter(0.3f);
        }
        
        g.setColour(segColor);
        g.fillRoundedRectangle(segBounds.reduced(1.0f), 4.0f);
        
        // Segment border
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.drawRoundedRectangle(segBounds.reduced(1.0f), 4.0f, 1.0f);
        
        // Segment text
        g.setColour(juce::Colours::white);
        g.setFont(11.0f);
        
        juce::String segText = juce::String(ScaleConfig::getRootNoteName(segment.rootNote)) + " " +
                               juce::String(ScaleConfig::getScaleTypeName(segment.scaleType));
        juce::String barsText = juce::String(segment.lengthBars) + (segment.lengthBars == 1 ? " bar" : " bars");
        
        auto textBounds = segBounds.reduced(4.0f);
        if (segmentWidth > 60) {
            g.drawText(segText, textBounds.removeFromTop(textBounds.getHeight() * 0.6f), 
                      juce::Justification::centred, true);
            g.setFont(9.0f);
            g.drawText(barsText, textBounds, juce::Justification::centred, true);
        } else {
            // Compact display for narrow segments
            g.setFont(9.0f);
            g.drawText(ScaleConfig::getRootNoteName(segment.rootNote), textBounds, 
                      juce::Justification::centred, true);
        }
        
        currentX += segmentWidth;
    }
    
    // Draw playback position indicator
    if (playbackPosition >= 0.0f && playbackPosition <= 1.0f) {
        float posX = timelineArea.getX() + playbackPosition * timelineArea.getWidth();
        
        // Draw glow around playhead
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.fillRect(posX - 5.0f, timelineArea.getY(), 10.0f, timelineArea.getHeight());
        
        // Main playhead line
        g.setColour(juce::Colours::white);
        g.drawLine(posX, timelineArea.getY(), posX, timelineArea.getBottom(), 2.0f);
    }
    
    // Draw segment change flash overlay
    if (segmentChangeFlash > 0.01f && lastActiveSegment >= 0) {
        auto segBounds = getSegmentBounds(lastActiveSegment);
        if (!segBounds.isEmpty()) {
            juce::Colour flashColor = getSegmentColor(lastActiveSegment).withAlpha(segmentChangeFlash * 0.4f);
            g.setColour(flashColor);
            g.fillRoundedRectangle(segBounds.expanded(4.0f), 6.0f);
        }
    }
}

void ScaleSequencerComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(25);  // Title
    bounds = bounds.reduced(10, 5);
    
    // Add button on the right
    addButton.setBounds(bounds.removeFromRight(35).reduced(0, 10));
}

//==============================================================================
void ScaleSequencerComponent::mouseDown(const juce::MouseEvent& event)
{
    auto pos = event.position;
    
    // Check if clicking on an edge for resizing
    int edge = getEdgeAtPoint(pos);
    if (edge >= 0) {
        draggingEdge = edge;
        dragStartX = pos.x;
        dragStartBars = patternModel.getScaleSequencer().segments[edge].lengthBars;
        return;
    }
    
    // Check if clicking on a segment
    int segment = getSegmentAtPoint(pos);
    if (segment >= 0) {
        selectedSegment = segment;
        showSegmentEditor(segment);
        repaint();
    } else {
        hideSegmentEditor();
        selectedSegment = -1;
        repaint();
    }
}

void ScaleSequencerComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (draggingEdge >= 0) {
        auto& config = patternModel.getScaleSequencer();
        if (draggingEdge < static_cast<int>(config.segments.size())) {
            // Calculate new bar length based on drag distance
            int totalBars = config.getTotalLengthBars();
            auto bounds = getLocalBounds().toFloat();
            bounds.removeFromTop(25.0f);
            bounds = bounds.reduced(10.0f, 5.0f);
            bounds.removeFromRight(40.0f);
            
            float pixelsPerBar = bounds.getWidth() / totalBars;
            float deltaX = event.position.x - dragStartX;
            int deltaBars = static_cast<int>(std::round(deltaX / pixelsPerBar));
            
            int newBars = juce::jlimit(1, 16, dragStartBars + deltaBars);
            config.segments[draggingEdge].lengthBars = newBars;
            
            repaint();
        }
    }
}

void ScaleSequencerComponent::mouseUp(const juce::MouseEvent& event)
{
    if (draggingEdge >= 0) {
        draggingEdge = -1;
        patternModel.sendChangeMessage();
    }
}

void ScaleSequencerComponent::mouseMove(const juce::MouseEvent& event)
{
    auto pos = event.position;
    
    // Check for edge hover (resize cursor)
    int edge = getEdgeAtPoint(pos);
    if (edge >= 0) {
        setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
    } else {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
    
    // Update hovered segment
    int newHovered = getSegmentAtPoint(pos);
    if (newHovered != hoveredSegment) {
        hoveredSegment = newHovered;
        repaint();
    }
}

//==============================================================================
void ScaleSequencerComponent::setPlaybackPosition(float normalizedPosition)
{
    if (playbackPosition != normalizedPosition) {
        playbackPosition = normalizedPosition;
        
        // Detect segment changes for flash effect
        auto& config = patternModel.getScaleSequencer();
        if (!config.segments.empty()) {
            int currentSegment = config.getSegmentIndexAtPosition(
                normalizedPosition * config.getTotalLengthBars()
            );
            
            if (currentSegment != lastActiveSegment && lastActiveSegment >= 0) {
                // Segment changed - trigger flash
                segmentChangeFlash = 1.0f;
            }
            lastActiveSegment = currentSegment;
        }
        
        // Decay flash
        segmentChangeFlash *= 0.85f;
        if (segmentChangeFlash < 0.01f) segmentChangeFlash = 0.0f;
        
        repaint();
    }
}

void ScaleSequencerComponent::refreshFromModel()
{
    repaint();
}

void ScaleSequencerComponent::addListener(Listener* listener)
{
    if (listener != nullptr) {
        listeners.push_back(listener);
    }
}

void ScaleSequencerComponent::removeListener(Listener* listener)
{
    listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
}

//==============================================================================
juce::Rectangle<float> ScaleSequencerComponent::getSegmentBounds(int index) const
{
    auto& config = patternModel.getScaleSequencer();
    if (index < 0 || index >= static_cast<int>(config.segments.size())) {
        return {};
    }
    
    auto bounds = getLocalBounds().toFloat();
    bounds.removeFromTop(25.0f);
    auto timelineArea = bounds.reduced(10.0f, 5.0f);
    timelineArea.removeFromRight(40.0f);
    
    int totalBars = config.getTotalLengthBars();
    if (totalBars <= 0) return {};
    
    float pixelsPerBar = timelineArea.getWidth() / totalBars;
    
    float startX = timelineArea.getX();
    for (int i = 0; i < index; ++i) {
        startX += config.segments[i].lengthBars * pixelsPerBar;
    }
    
    float width = config.segments[index].lengthBars * pixelsPerBar;
    
    return juce::Rectangle<float>(startX, timelineArea.getY(), width, timelineArea.getHeight());
}

int ScaleSequencerComponent::getSegmentAtPoint(juce::Point<float> point) const
{
    auto& config = patternModel.getScaleSequencer();
    
    for (size_t i = 0; i < config.segments.size(); ++i) {
        auto segBounds = getSegmentBounds(static_cast<int>(i));
        if (segBounds.contains(point)) {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

int ScaleSequencerComponent::getEdgeAtPoint(juce::Point<float> point) const
{
    auto& config = patternModel.getScaleSequencer();
    const float edgeThreshold = 8.0f;
    
    for (size_t i = 0; i < config.segments.size(); ++i) {
        auto segBounds = getSegmentBounds(static_cast<int>(i));
        float rightEdge = segBounds.getRight();
        
        if (std::abs(point.x - rightEdge) < edgeThreshold &&
            point.y >= segBounds.getY() && point.y <= segBounds.getBottom()) {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

//==============================================================================
void ScaleSequencerComponent::showSegmentEditor(int segmentIndex)
{
    hideSegmentEditor();
    
    auto& config = patternModel.getScaleSequencer();
    if (segmentIndex < 0 || segmentIndex >= static_cast<int>(config.segments.size())) {
        return;
    }
    
    const auto& segment = config.segments[segmentIndex];
    
    // Create popup editor - use a simple component with paint override via lambda
    class PopupPanel : public juce::Component {
    public:
        void paint(juce::Graphics& g) override {
            g.fillAll(juce::Colour(0xff333333));
            g.setColour(juce::Colour(0xff555555));
            g.drawRect(getLocalBounds(), 2);
        }
    };
    
    // Wider horizontal layout
    popupEditor = std::make_unique<PopupPanel>();
    popupEditor->setSize(420, 90);
    
    // Row 1: Key, Scale, Bars - all side by side
    int y = 10;
    int x = 10;
    int comboHeight = 28;
    
    // Key combo
    auto* rootLabel = new juce::Label("", "Key:");
    rootLabel->setBounds(x, y, 35, comboHeight);
    popupEditor->addAndMakeVisible(rootLabel);
    x += 35;
    
    popupRootCombo = std::make_unique<juce::ComboBox>();
    for (int i = 0; i < 12; ++i) {
        popupRootCombo->addItem(ScaleConfig::getRootNoteName(static_cast<RootNote>(i)), i + 1);
    }
    popupRootCombo->setSelectedId(static_cast<int>(segment.rootNote) + 1, juce::dontSendNotification);
    popupRootCombo->onChange = [this, segmentIndex]() { onSegmentChanged(segmentIndex); };
    popupRootCombo->setBounds(x, y, 55, comboHeight);
    popupEditor->addAndMakeVisible(popupRootCombo.get());
    x += 60;
    
    // Scale combo
    auto* scaleLabel = new juce::Label("", "Scale:");
    scaleLabel->setBounds(x, y, 45, comboHeight);
    popupEditor->addAndMakeVisible(scaleLabel);
    x += 45;
    
    popupScaleCombo = std::make_unique<juce::ComboBox>();
    for (int i = 0; i < NUM_SCALE_TYPES; ++i) {
        popupScaleCombo->addItem(ScaleConfig::getScaleTypeName(static_cast<ScaleType>(i)), i + 1);
    }
    popupScaleCombo->setSelectedId(static_cast<int>(segment.scaleType) + 1, juce::dontSendNotification);
    popupScaleCombo->onChange = [this, segmentIndex]() { onSegmentChanged(segmentIndex); };
    popupScaleCombo->setBounds(x, y, 115, comboHeight);
    popupEditor->addAndMakeVisible(popupScaleCombo.get());
    x += 120;
    
    // Bars combo
    auto* barsLabel = new juce::Label("", "Bars:");
    barsLabel->setBounds(x, y, 40, comboHeight);
    popupEditor->addAndMakeVisible(barsLabel);
    x += 40;
    
    popupBarsCombo = std::make_unique<juce::ComboBox>();
    for (int i = 1; i <= 16; ++i) {
        popupBarsCombo->addItem(juce::String(i) + (i == 1 ? " bar" : " bars"), i);
    }
    popupBarsCombo->setSelectedId(segment.lengthBars, juce::dontSendNotification);
    popupBarsCombo->onChange = [this, segmentIndex]() { onSegmentChanged(segmentIndex); };
    popupBarsCombo->setBounds(x, y, 70, comboHeight);
    popupEditor->addAndMakeVisible(popupBarsCombo.get());
    
    // Row 2: Delete (small, red, left) and Close (prominent, right)
    y += comboHeight + 10;
    
    // Delete button - small, red-tinted, on the left (less accessible = safer)
    popupDeleteButton = std::make_unique<juce::TextButton>("X");
    popupDeleteButton->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff8B4444));  // Muted red
    popupDeleteButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    popupDeleteButton->setTooltip("Delete this segment");
    popupDeleteButton->onClick = [this, segmentIndex]() { 
        onDeleteSegment(segmentIndex); 
    };
    popupDeleteButton->setBounds(10, y, 40, 30);  // Small square button
    popupEditor->addAndMakeVisible(popupDeleteButton.get());
    
    // Close button - prominent, on the right (easy to reach)
    popupCloseButton = std::make_unique<juce::TextButton>("Close");
    popupCloseButton->onClick = [this]() { hideSegmentEditor(); };
    popupCloseButton->setBounds(60, y, 350, 30);  // Takes most of the width
    popupEditor->addAndMakeVisible(popupCloseButton.get());
    
    // Position popup near the segment
    auto segBounds = getSegmentBounds(segmentIndex);
    int popupX = static_cast<int>(segBounds.getCentreX() - 210);
    int popupY = static_cast<int>(segBounds.getBottom() + 5);
    
    // Keep popup within component bounds
    popupX = juce::jlimit(5, getWidth() - 425, popupX);
    popupY = juce::jlimit(5, getHeight() - 95, popupY);
    
    popupEditor->setTopLeftPosition(popupX, popupY);
    
    addAndMakeVisible(popupEditor.get());
}

void ScaleSequencerComponent::hideSegmentEditor()
{
    if (popupEditor != nullptr) {
        removeChildComponent(popupEditor.get());
        popupEditor.reset();
        popupRootCombo.reset();
        popupScaleCombo.reset();
        popupBarsCombo.reset();
        popupDeleteButton.reset();
        popupCloseButton.reset();
    }
    selectedSegment = -1;
    repaint();
}

void ScaleSequencerComponent::onAddSegment()
{
    auto& config = patternModel.getScaleSequencer();
    
    if (static_cast<int>(config.segments.size()) >= ScaleSequencerConfig::MAX_SEGMENTS) {
        return;  // At max capacity
    }
    
    // Add a new segment with default values (or copy from last segment)
    ScaleSequenceSegment newSeg;
    if (!config.segments.empty()) {
        newSeg = config.segments.back();  // Copy last segment's settings
    }
    newSeg.lengthBars = 2;  // Default to 2 bars
    
    config.segments.push_back(newSeg);
    
    patternModel.sendChangeMessage();
    repaint();
}

void ScaleSequencerComponent::onDeleteSegment(int index)
{
    auto& config = patternModel.getScaleSequencer();
    
    if (index >= 0 && index < static_cast<int>(config.segments.size())) {
        // Don't delete the last segment
        if (config.segments.size() > 1) {
            config.segments.erase(config.segments.begin() + index);
            hideSegmentEditor();
            patternModel.sendChangeMessage();
        }
    }
}

void ScaleSequencerComponent::onSegmentChanged(int index)
{
    auto& config = patternModel.getScaleSequencer();
    
    if (index >= 0 && index < static_cast<int>(config.segments.size())) {
        auto& segment = config.segments[index];
        
        if (popupRootCombo) {
            segment.rootNote = static_cast<RootNote>(popupRootCombo->getSelectedId() - 1);
        }
        if (popupScaleCombo) {
            segment.scaleType = static_cast<ScaleType>(popupScaleCombo->getSelectedId() - 1);
        }
        if (popupBarsCombo) {
            segment.lengthBars = popupBarsCombo->getSelectedId();
        }
        
        patternModel.sendChangeMessage();
        repaint();
    }
}

juce::Colour ScaleSequencerComponent::getSegmentColor(int index) const
{
    // Muted earth tones / desaturated colors that won't conflict with main channel colors
    // Main channels use: Copper, Steel Blue, Purple, Teal
    // Scale sequencer uses: Browns, grays, muted warm/cool tones
    static const juce::Colour colors[] = {
        juce::Colour(0xff8B7355),  // Muted brown/taupe
        juce::Colour(0xff6B7B8C),  // Slate gray
        juce::Colour(0xff9C8B7A),  // Warm gray/beige
        juce::Colour(0xff5D6D7E),  // Cool gray-blue
        juce::Colour(0xffA68B6A),  // Tan/khaki
        juce::Colour(0xff7B8A8B),  // Neutral gray
        juce::Colour(0xff8E7B6B),  // Dusty rose-brown
        juce::Colour(0xff6A7B6A),  // Muted sage (desaturated, won't confuse with teal)
    };
    
    return colors[index % 8];
}

} // namespace SquareBeats

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SquareBeatsAudioProcessorEditor::SquareBeatsAudioProcessorEditor (SquareBeatsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Create the gate flash overlay (behind everything else)
    gateFlashOverlay = std::make_unique<SquareBeats::GateFlashOverlay>(
        audioProcessor.getPatternModel(),
        audioProcessor.getVisualFeedbackState()
    );
    addAndMakeVisible(gateFlashOverlay.get());
    
    // Create the sequencing plane component
    sequencingPlane = std::make_unique<SquareBeats::SequencingPlaneComponent>(
        audioProcessor.getPatternModel()
    );
    sequencingPlane->setVisualFeedbackState(&audioProcessor.getVisualFeedbackState());
    addAndMakeVisible(sequencingPlane.get());
    
    // Create pitch sequencer component (as overlay on sequencing plane)
    pitchSequencer = std::make_unique<SquareBeats::PitchSequencerComponent>(
        audioProcessor.getPatternModel()
    );
    addAndMakeVisible(pitchSequencer.get());
    
    // Create color selector
    colorSelector = std::make_unique<SquareBeats::ColorSelectorComponent>(
        audioProcessor.getPatternModel()
    );
    colorSelector->setVisualFeedbackState(&audioProcessor.getVisualFeedbackState());
    colorSelector->addListener(this);
    addAndMakeVisible(colorSelector.get());
    
    // Create color config panel
    colorConfigPanel = std::make_unique<SquareBeats::ColorConfigPanel>(
        audioProcessor.getPatternModel()
    );
    addAndMakeVisible(colorConfigPanel.get());
    
    // Create loop length selector
    loopLengthSelector = std::make_unique<SquareBeats::LoopLengthSelector>(
        audioProcessor.getPatternModel()
    );
    addAndMakeVisible(loopLengthSelector.get());
    
    // Create time signature controls
    timeSignatureControls = std::make_unique<SquareBeats::TimeSignatureControls>(
        audioProcessor.getPatternModel()
    );
    addAndMakeVisible(timeSignatureControls.get());
    
    // Create scale controls
    scaleControls = std::make_unique<SquareBeats::ScaleControls>(
        audioProcessor.getPatternModel()
    );
    addAndMakeVisible(scaleControls.get());
    
    // Create clear all button (top bar)
    clearAllButton.setButtonText("Clear All");
    clearAllButton.onClick = [this]() { onClearAllClicked(); };
    addAndMakeVisible(clearAllButton);
    
    // Create control buttons
    controlButtons = std::make_unique<SquareBeats::ControlButtons>(
        audioProcessor.getPatternModel()
    );
    controlButtons->addListener(this);
    addAndMakeVisible(controlButtons.get());
    
    // Create play mode controls
    playModeControls = std::make_unique<SquareBeats::PlayModeControls>(
        audioProcessor.getPatternModel()
    );
    addAndMakeVisible(playModeControls.get());
    
    // Create scale sequencer component (initially hidden)
    scaleSequencer = std::make_unique<SquareBeats::ScaleSequencerComponent>(
        audioProcessor.getPatternModel()
    );
    scaleSequencer->setVisible(false);
    addAndMakeVisible(scaleSequencer.get());
    
    // Scale sequencer toggle button
    scaleSeqToggle.setButtonText("Scale Seq");
    scaleSeqToggle.setClickingTogglesState(true);
    scaleSeqToggle.onClick = [this]() {
        auto& scaleSeqConfig = audioProcessor.getPatternModel().getScaleSequencer();
        scaleSeqConfig.enabled = scaleSeqToggle.getToggleState();
        scaleSequencer->setVisible(scaleSeqConfig.enabled);
        
        // Enable/disable scale controls based on scale sequencer state
        if (scaleControls != nullptr) {
            scaleControls->setControlsEnabled(!scaleSeqConfig.enabled);
            if (!scaleSeqConfig.enabled) {
                scaleControls->setActiveScale(nullptr);  // Restore static scale display
            }
        }
        
        resized();
        audioProcessor.getPatternModel().sendChangeMessage();
    };
    addAndMakeVisible(scaleSeqToggle);
    
    // Listen to pattern model changes
    audioProcessor.getPatternModel().addChangeListener(this);
    
    // Start timer for playback position updates (60 FPS for smooth visual effects)
    startTimerHz(60);
    
    // Set initial size and make resizable with constraints
    setSize (1000, 700);
    setResizable(true, true);
    setResizeLimits(800, 600, 2000, 1500);  // Min: 800x600, Max: 2000x1500
}

SquareBeatsAudioProcessorEditor::~SquareBeatsAudioProcessorEditor()
{
    // Stop timer
    stopTimer();
    
    // Remove listeners
    audioProcessor.getPatternModel().removeChangeListener(this);
    
    if (colorSelector != nullptr)
    {
        colorSelector->removeListener(this);
    }
    
    if (controlButtons != nullptr)
    {
        controlButtons->removeListener(this);
    }
}

//==============================================================================
void SquareBeatsAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill the background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SquareBeatsAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Top bar: Loop length selector, time signature controls, scale controls, scale seq toggle, and clear all button
    auto topBar = bounds.removeFromTop(60);
    loopLengthSelector->setBounds(topBar.removeFromLeft(250));
    topBar.removeFromLeft(10); // Spacing
    timeSignatureControls->setBounds(topBar.removeFromLeft(200));
    topBar.removeFromLeft(10); // Spacing
    scaleControls->setBounds(topBar.removeFromLeft(290));
    topBar.removeFromLeft(10); // Spacing
    scaleSeqToggle.setBounds(topBar.removeFromLeft(70).reduced(0, 15));
    topBar.removeFromLeft(10); // Spacing
    clearAllButton.setBounds(topBar.removeFromLeft(80).reduced(0, 15));
    
    // Right panel: Color selector, config panel, and control buttons
    auto rightPanel = bounds.removeFromRight(250);
    
    colorSelector->setBounds(rightPanel.removeFromTop(50));
    rightPanel.removeFromTop(10); // Spacing
    colorConfigPanel->setBounds(rightPanel.removeFromTop(200));
    rightPanel.removeFromTop(10); // Spacing
    controlButtons->setBounds(rightPanel.removeFromTop(170)); // Increased height for pitch seq length dropdown
    rightPanel.removeFromTop(10); // Spacing
    playModeControls->setBounds(rightPanel.removeFromTop(200)); // Play mode controls with XY pad
    
    // Scale sequencer overlay (when visible, takes bottom portion of main area)
    auto& scaleSeqConfig = audioProcessor.getPatternModel().getScaleSequencer();
    if (scaleSeqConfig.enabled && scaleSequencer->isVisible()) {
        scaleSequencer->setBounds(bounds.removeFromBottom(120));
        bounds.removeFromBottom(5);
    }
    
    // Main area: Sequencing plane with pitch sequencer overlay
    gateFlashOverlay->setBounds(bounds);  // Flash overlay behind sequencing plane
    sequencingPlane->setBounds(bounds);
    pitchSequencer->setBounds(bounds);  // Same bounds as sequencing plane (overlay)
}

//==============================================================================
void SquareBeatsAudioProcessorEditor::colorChannelSelected(int colorChannelId)
{
    // Update the sequencing plane's selected color
    if (sequencingPlane != nullptr)
    {
        sequencingPlane->setSelectedColorChannel(colorChannelId);
    }
    
    // Update the pitch sequencer to show the selected color's waveform
    if (pitchSequencer != nullptr)
    {
        pitchSequencer->setSelectedColorChannel(colorChannelId);
    }
    
    // Update the color config panel to show the selected color's settings
    if (colorConfigPanel != nullptr)
    {
        colorConfigPanel->setColorChannel(colorChannelId);
    }
    
    // Update the control buttons to know which color to clear and refresh pitch seq length
    if (controlButtons != nullptr)
    {
        controlButtons->setSelectedColorChannel(colorChannelId);
        controlButtons->refreshFromModel(); // Update pitch seq length dropdown for this color
    }
}

void SquareBeatsAudioProcessorEditor::pitchSequencerVisibilityChanged(bool isVisible)
{
    // Update the pitch sequencer component visibility
    if (pitchSequencer != nullptr)
    {
        pitchSequencer->updateVisibility();
    }
}

void SquareBeatsAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // Pattern model has changed - update all UI components
    if (sequencingPlane != nullptr)
    {
        sequencingPlane->repaint();
    }
    
    if (pitchSequencer != nullptr)
    {
        pitchSequencer->repaint();
    }
    
    if (colorConfigPanel != nullptr)
    {
        colorConfigPanel->refreshFromModel();
    }
    
    if (loopLengthSelector != nullptr)
    {
        loopLengthSelector->refreshFromModel();
    }
    
    if (timeSignatureControls != nullptr)
    {
        timeSignatureControls->refreshFromModel();
    }
    
    if (scaleControls != nullptr)
    {
        scaleControls->refreshFromModel();
    }
    
    if (controlButtons != nullptr)
    {
        controlButtons->refreshFromModel();
    }
    
    if (playModeControls != nullptr)
    {
        playModeControls->refreshFromModel();
    }
    
    if (scaleSequencer != nullptr)
    {
        scaleSequencer->refreshFromModel();
    }
    
    // Update scale sequencer toggle state and scale controls enabled state
    auto& scaleSeqConfig = audioProcessor.getPatternModel().getScaleSequencer();
    scaleSeqToggle.setToggleState(scaleSeqConfig.enabled, juce::dontSendNotification);
    scaleSequencer->setVisible(scaleSeqConfig.enabled);
    
    if (scaleControls != nullptr)
    {
        scaleControls->setControlsEnabled(!scaleSeqConfig.enabled);
    }
}

void SquareBeatsAudioProcessorEditor::timerCallback()
{
    // Update visual feedback time (milliseconds since start)
    static auto startTime = juce::Time::getMillisecondCounterHiRes();
    float currentTimeMs = static_cast<float>(juce::Time::getMillisecondCounterHiRes() - startTime);
    audioProcessor.getVisualFeedbackState().updateTime(currentTimeMs);
    audioProcessor.getBeatPulseState().updateTime(currentTimeMs);
    
    // Repaint the gate flash overlay for smooth animation
    if (gateFlashOverlay != nullptr)
    {
        gateFlashOverlay->repaint();
    }
    
    // Update beat pulse intensity and repaint sequencing plane
    if (sequencingPlane != nullptr)
    {
        float beatPulse = audioProcessor.getBeatPulseState().getPulseIntensity();
        sequencingPlane->setBeatPulseIntensity(beatPulse);
    }
    
    // Repaint color selector for activity indicators
    if (colorSelector != nullptr)
    {
        colorSelector->repaint();
    }
    
    // Update playback position from the audio processor
    if (sequencingPlane != nullptr)
    {
        float normalizedPosition = audioProcessor.getPlaybackEngine().getNormalizedPlaybackPosition();
        sequencingPlane->setPlaybackPosition(normalizedPosition);
    }
    
    // Update pitch sequencer playback position for the selected color
    if (pitchSequencer != nullptr && colorSelector != nullptr)
    {
        int selectedColor = colorSelector->getSelectedColorChannel();
        float pitchSeqPosition = audioProcessor.getPlaybackEngine().getNormalizedPitchSeqPosition(selectedColor);
        pitchSequencer->setPlaybackPosition(pitchSeqPosition);
    }
    
    // Update scale sequencer playback position and active scale display
    auto& scaleSeqConfig = audioProcessor.getPatternModel().getScaleSequencer();
    if (scaleSeqConfig.enabled)
    {
        // Update scale sequencer position
        if (scaleSequencer != nullptr && scaleSequencer->isVisible())
        {
            float scaleSeqPosition = audioProcessor.getPlaybackEngine().getNormalizedScaleSeqPosition();
            scaleSequencer->setPlaybackPosition(scaleSeqPosition);
        }
        
        // Update active scale display in header
        if (scaleControls != nullptr)
        {
            double positionBars = audioProcessor.getPlaybackEngine().getPositionInBars();
            SquareBeats::ScaleConfig activeScale = scaleSeqConfig.getScaleAtPosition(positionBars);
            scaleControls->setActiveScale(&activeScale);
        }
    }
    else
    {
        // Clear active scale display when scale sequencer is disabled
        if (scaleControls != nullptr)
        {
            scaleControls->setActiveScale(nullptr);
        }
    }
}

void SquareBeatsAudioProcessorEditor::onClearAllClicked()
{
    auto& patternModel = audioProcessor.getPatternModel();
    
    // Clear all squares from all 4 color channels
    for (int i = 0; i < 4; ++i)
    {
        patternModel.clearColorChannel(i);
        
        // Clear pitch sequencer waveform for this color
        auto& colorConfig = patternModel.getColorConfig(i);
        std::fill(colorConfig.pitchWaveform.begin(), colorConfig.pitchWaveform.end(), 0.0f);
    }
    
    // Trigger UI update
    patternModel.sendChangeMessage();
}

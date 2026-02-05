#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"
#include "HelpAboutDialog.h"

//==============================================================================
SquareBeatsAudioProcessorEditor::SquareBeatsAudioProcessorEditor (SquareBeatsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Load the logo image from binary data
    logoImage = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);
    
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
    // Connect the editing mode change callback to update pitch sequencer visibility
    colorConfigPanel->onEditingModeChanged = [this](bool isPitchMode) {
        if (pitchSequencer != nullptr)
        {
            pitchSequencer->updateVisibility();
        }
    };
    addAndMakeVisible(colorConfigPanel.get());
    
    // Create loop length selector (now a dropdown)
    loopLengthSelector = std::make_unique<SquareBeats::LoopLengthSelector>(
        audioProcessor.getPatternModel()
    );
    addAndMakeVisible(loopLengthSelector.get());
    
    // Create scale controls
    scaleControls = std::make_unique<SquareBeats::ScaleControls>(
        audioProcessor.getPatternModel()
    );
    addAndMakeVisible(scaleControls.get());
    
    // Create clear all button (top bar)
    clearAllButton.setButtonText("Clear All");
    clearAllButton.onClick = [this]() { onClearAllClicked(); };
    addAndMakeVisible(clearAllButton);
    
    // Create preset controls (top bar)
    presetComboBox.setTextWhenNothingSelected("Select Preset...");
    presetComboBox.onChange = [this]() { onPresetSelected(); };
    addAndMakeVisible(presetComboBox);
    
    savePresetButton.setButtonText("Save");
    savePresetButton.onClick = [this]() { onSavePresetClicked(); };
    addAndMakeVisible(savePresetButton);
    
    deletePresetButton.setButtonText("Delete");
    deletePresetButton.onClick = [this]() { onDeletePresetClicked(); };
    addAndMakeVisible(deletePresetButton);
    
    // Populate preset list
    refreshPresetList();
    
    // Create control buttons
    controlButtons = std::make_unique<SquareBeats::ControlButtons>(
        audioProcessor.getPatternModel()
    );
    controlButtons->addListener(this);
    addAndMakeVisible(controlButtons.get());
    
    // Create play mode buttons (for top bar)
    playModeButtons = std::make_unique<SquareBeats::PlayModeButtons>(
        audioProcessor.getPatternModel()
    );
    playModeButtons->onProbabilityModeChanged = [this](bool /*isProbability*/) {
        updateContextSensitiveControls();
    };
    playModeButtons->onPlayModeChanged = [this]() {
        // Reset transport position when play mode changes to stay on beat
        audioProcessor.getPlaybackEngine().resetPlaybackPosition();
    };
    addAndMakeVisible(playModeButtons.get());
    
    // Create play mode XY pad (for side panel, only visible in probability mode)
    playModeXYPad = std::make_unique<SquareBeats::PlayModeXYPad>(
        audioProcessor.getPatternModel()
    );
    addAndMakeVisible(playModeXYPad.get());
    
    // Create scale sequencer component (initially hidden)
    scaleSequencer = std::make_unique<SquareBeats::ScaleSequencerComponent>(
        audioProcessor.getPatternModel()
    );
    scaleSequencer->setVisible(false);
    addAndMakeVisible(scaleSequencer.get());
    
    // Scale sequencer toggle button
    scaleSeqToggle.setButtonText("Enable Scale Sequencer");
    scaleSeqToggle.setClickingTogglesState(true);
    scaleSeqToggle.onClick = [this]() {
        auto& scaleSeqConfig = audioProcessor.getPatternModel().getScaleSequencer();
        scaleSeqConfig.enabled = scaleSeqToggle.getToggleState();
        scaleSequencer->setVisible(scaleSeqConfig.enabled);
        
        // Update button text based on state
        scaleSeqToggle.setButtonText(scaleSeqConfig.enabled ? "Disable Scale Sequencer" : "Enable Scale Sequencer");
        
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
    
    // Initial context-sensitive controls update
    updateContextSensitiveControls();
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
    
    // Draw the logo at the bottom of the right panel
    if (logoImage.isValid())
    {
        auto logoBounds = getLogoBounds();
        
        // Draw with high quality scaling
        g.drawImage(logoImage, 
                    logoBounds.toFloat(),
                    juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
        
        // Draw a subtle hover effect if mouse is over logo
        if (logoBounds.contains(getMouseXYRelative()))
        {
            g.setColour(juce::Colours::white.withAlpha(0.1f));
            g.fillRect(logoBounds);
        }
    }
}

void SquareBeatsAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Standard button height for consistency across the app
    const int standardButtonHeight = 30;
    
    // Right panel: All controls from top to bottom, logo at bottom
    auto rightPanel = bounds.removeFromRight(280);
    auto rightPanelOriginal = rightPanel;  // Save original bounds for logo positioning
    
    // === TOP SECTION: Play Mode Controls ===
    rightPanel.removeFromTop(5); // Top padding
    
    // Play mode buttons
    playModeButtons->setBounds(rightPanel.removeFromTop(40).reduced(5, 0));
    rightPanel.removeFromTop(10); // Section spacing
    
    // === GLOBAL CONTROLS ===
    
    // Preset controls (top bar)
    auto presetArea = rightPanel.removeFromTop(40).reduced(5, 0);
    int presetButtonWidth = 60;
    deletePresetButton.setBounds(presetArea.removeFromRight(presetButtonWidth));
    presetArea.removeFromRight(5);
    savePresetButton.setBounds(presetArea.removeFromRight(presetButtonWidth));
    presetArea.removeFromRight(5);
    presetComboBox.setBounds(presetArea);
    rightPanel.removeFromTop(5); // Spacing
    
    // Loop length selector
    loopLengthSelector->setBounds(rightPanel.removeFromTop(40));
    rightPanel.removeFromTop(5); // Spacing
    
    // Clear All button
    clearAllButton.setBounds(rightPanel.removeFromTop(standardButtonHeight).reduced(5, 0));
    rightPanel.removeFromTop(10); // Section spacing
    
    // === MIDDLE SECTION: Color Controls ===
    
    // Color selector
    colorSelector->setBounds(rightPanel.removeFromTop(50));
    rightPanel.removeFromTop(10); // Spacing
    
    // Color config panel (context-sensitive: squares or pitch mode)
    colorConfigPanel->setBounds(rightPanel.removeFromTop(310));
    
    rightPanel.removeFromTop(10); // Spacing
    controlButtons->setBounds(rightPanel.removeFromTop(0));  // ControlButtons is now empty
    
    // Show XY pad only in probability mode
    auto& playModeConfig = audioProcessor.getPatternModel().getPlayModeConfig();
    if (playModeConfig.mode == SquareBeats::PLAY_PROBABILITY)
    {
        rightPanel.removeFromTop(10); // Spacing
        playModeXYPad->setBounds(rightPanel.removeFromTop(180));
    }
    
    rightPanel.removeFromTop(10); // Section spacing
    
    // === SCALE CONTROLS ===
    
    // Scale controls (root and scale dropdowns)
    scaleControls->setBounds(rightPanel.removeFromTop(40));
    rightPanel.removeFromTop(5); // Spacing
    
    // Scale Seq toggle button
    scaleSeqToggle.setBounds(rightPanel.removeFromTop(standardButtonHeight).reduced(5, 0));
    
    // === LOGO AT BOTTOM ===
    
    // Logo at the bottom of right panel (maintain aspect ratio)
    int logoHeight = 50;
    int logoWidth = logoImage.isValid() ? 
        static_cast<int>(logoImage.getWidth() * (static_cast<float>(logoHeight) / logoImage.getHeight())) : 50;
    
    // Position logo at bottom, centered horizontally in right panel
    auto logoArea = rightPanel.removeFromBottom(logoHeight + 10);
    int xOffset = (rightPanelOriginal.getWidth() - logoWidth) / 2;
    logoClickArea = juce::Rectangle<int>(rightPanelOriginal.getX() + xOffset, 
                                          logoArea.getY() + 5, 
                                          logoWidth, 
                                          logoHeight);
    
    // Scale sequencer overlay (when visible, takes bottom portion of main area)
    auto& scaleSeqConfig = audioProcessor.getPatternModel().getScaleSequencer();
    if (scaleSeqConfig.enabled && scaleSequencer->isVisible()) {
        scaleSequencer->setBounds(bounds.removeFromBottom(120));
        bounds.removeFromBottom(5);
    }
    
    // Main area: Sequencing plane with pitch sequencer overlay (full height on left)
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
    
    if (scaleControls != nullptr)
    {
        scaleControls->refreshFromModel();
    }
    
    if (controlButtons != nullptr)
    {
        controlButtons->refreshFromModel();
    }
    
    if (playModeButtons != nullptr)
    {
        playModeButtons->refreshFromModel();
    }
    
    if (playModeXYPad != nullptr)
    {
        playModeXYPad->refreshFromModel();
    }
    
    if (scaleSequencer != nullptr)
    {
        scaleSequencer->refreshFromModel();
    }
    
    // Update scale sequencer toggle state and scale controls enabled state
    auto& scaleSeqConfig = audioProcessor.getPatternModel().getScaleSequencer();
    scaleSeqToggle.setToggleState(scaleSeqConfig.enabled, juce::dontSendNotification);
    scaleSeqToggle.setButtonText(scaleSeqConfig.enabled ? "Disable Scale Sequencer" : "Enable Scale Sequencer");
    scaleSequencer->setVisible(scaleSeqConfig.enabled);
    
    if (scaleControls != nullptr)
    {
        scaleControls->setControlsEnabled(!scaleSeqConfig.enabled);
    }
    
    // Update context-sensitive controls
    updateContextSensitiveControls();
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
        
        // Update per-color playback positions
        for (int colorId = 0; colorId < 4; ++colorId)
        {
            float colorPos = audioProcessor.getPlaybackEngine().getNormalizedPlaybackPositionForColor(colorId);
            sequencingPlane->setColorPlaybackPosition(colorId, colorPos);
        }
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

void SquareBeatsAudioProcessorEditor::updateContextSensitiveControls()
{
    // Show/hide XY pad based on probability mode
    auto& playModeConfig = audioProcessor.getPatternModel().getPlayModeConfig();
    bool showXYPad = (playModeConfig.mode == SquareBeats::PLAY_PROBABILITY);
    
    if (playModeXYPad != nullptr)
    {
        playModeXYPad->setVisible(showXYPad);
    }
    
    // Trigger layout update
    resized();
    repaint();
}

void SquareBeatsAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    // Check if click is on the logo
    if (logoClickArea.contains(event.getPosition()))
    {
        SquareBeats::HelpAboutDialog::show(this);
    }
}

juce::Rectangle<int> SquareBeatsAudioProcessorEditor::getLogoBounds() const
{
    return logoClickArea;
}

//==============================================================================
// Preset Management

void SquareBeatsAudioProcessorEditor::refreshPresetList()
{
    presetComboBox.clear(juce::dontSendNotification);
    
    auto presetList = audioProcessor.getPresetList();
    
    for (int i = 0; i < presetList.size(); ++i)
    {
        presetComboBox.addItem(presetList[i], i + 1);
    }
}

void SquareBeatsAudioProcessorEditor::onPresetSelected()
{
    int selectedId = presetComboBox.getSelectedId();
    if (selectedId > 0)
    {
        juce::String presetName = presetComboBox.getItemText(selectedId - 1);
        
        if (audioProcessor.loadPreset(presetName))
        {
            // Preset loaded successfully - UI will update via change listener
            juce::Logger::writeToLog("Loaded preset: " + presetName);
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Load Failed",
                "Failed to load preset '" + presetName + "'",
                "OK"
            );
        }
    }
}

void SquareBeatsAudioProcessorEditor::onSavePresetClicked()
{
    // Create a lambda to handle the save logic
    auto performSave = [this]()
    {
        auto* window = new juce::AlertWindow("Save Preset", 
                                             "Enter a name for the preset:", 
                                             juce::AlertWindow::NoIcon);
        
        window->addTextEditor("presetName", "", "Preset Name:");
        window->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
        
        window->enterModalState(true, juce::ModalCallbackFunction::create(
            [this, window](int result)
            {
                if (result == 1)
                {
                    juce::String presetName = window->getTextEditorContents("presetName").trim();
                    
                    if (presetName.isEmpty())
                    {
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::AlertWindow::WarningIcon,
                            "Invalid Name",
                            "Preset name cannot be empty.",
                            "OK"
                        );
                        delete window;
                        return;
                    }
                    
                    // Check if preset already exists
                    if (audioProcessor.presetExists(presetName))
                    {
                        // Ask for overwrite confirmation
                        juce::AlertWindow::showOkCancelBox(
                            juce::AlertWindow::QuestionIcon,
                            "Overwrite Preset",
                            "A preset named '" + presetName + "' already exists. Overwrite it?",
                            "Overwrite",
                            "Cancel",
                            nullptr,
                            juce::ModalCallbackFunction::create([this, presetName](int overwriteResult)
                            {
                                if (overwriteResult == 1)
                                {
                                    // Save the preset
                                    if (audioProcessor.savePreset(presetName))
                                    {
                                        refreshPresetList();
                                        
                                        // Select the newly saved preset
                                        for (int i = 0; i < presetComboBox.getNumItems(); ++i)
                                        {
                                            if (presetComboBox.getItemText(i) == presetName)
                                            {
                                                presetComboBox.setSelectedId(i + 1, juce::dontSendNotification);
                                                break;
                                            }
                                        }
                                        
                                        juce::Logger::writeToLog("Saved preset: " + presetName);
                                    }
                                    else
                                    {
                                        juce::AlertWindow::showMessageBoxAsync(
                                            juce::AlertWindow::WarningIcon,
                                            "Save Failed",
                                            "Failed to save preset '" + presetName + "'",
                                            "OK"
                                        );
                                    }
                                }
                            })
                        );
                        delete window;
                        return;
                    }
                    
                    // Save the preset (no overwrite needed)
                    if (audioProcessor.savePreset(presetName))
                    {
                        refreshPresetList();
                        
                        // Select the newly saved preset
                        for (int i = 0; i < presetComboBox.getNumItems(); ++i)
                        {
                            if (presetComboBox.getItemText(i) == presetName)
                            {
                                presetComboBox.setSelectedId(i + 1, juce::dontSendNotification);
                                break;
                            }
                        }
                        
                        juce::Logger::writeToLog("Saved preset: " + presetName);
                    }
                    else
                    {
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::AlertWindow::WarningIcon,
                            "Save Failed",
                            "Failed to save preset '" + presetName + "'",
                            "OK"
                        );
                    }
                }
                
                delete window;
            }
        ), true);  // deleteWhenDismissed = true
    };
    
    performSave();
}

void SquareBeatsAudioProcessorEditor::onDeletePresetClicked()
{
    int selectedId = presetComboBox.getSelectedId();
    if (selectedId <= 0)
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon,
            "No Preset Selected",
            "Please select a preset to delete.",
            "OK"
        );
        return;
    }
    
    juce::String presetName = presetComboBox.getItemText(selectedId - 1);
    
    // Confirm deletion
    juce::AlertWindow::showOkCancelBox(
        juce::AlertWindow::QuestionIcon,
        "Delete Preset",
        "Are you sure you want to delete the preset '" + presetName + "'?",
        "Delete",
        "Cancel",
        nullptr,
        juce::ModalCallbackFunction::create([this, presetName](int result) {
            if (result == 1)
            {
                // Delete the preset
                if (audioProcessor.deletePreset(presetName))
                {
                    refreshPresetList();
                    presetComboBox.setSelectedId(0, juce::dontSendNotification);
                    juce::Logger::writeToLog("Deleted preset: " + presetName);
                }
                else
                {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Delete Failed",
                        "Failed to delete preset '" + presetName + "'",
                        "OK"
                    );
                }
            }
        })
    );
}

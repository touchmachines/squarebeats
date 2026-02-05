#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SquareBeatsAudioProcessor::SquareBeatsAudioProcessor()
     : AudioProcessor (BusesProperties()
                       // Ableton Live requires synths to have audio output
                       // We output silence but this makes the plugin loadable
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )
{
    // Initialize PlaybackEngine with PatternModel
    playbackEngine.setPatternModel(&patternModel);
    
    // Connect visual feedback state to playback engine
    playbackEngine.setVisualFeedbackState(&visualFeedbackState);
    
    // Create factory presets if needed
    presetManager.createFactoryPresetsIfNeeded();
}

SquareBeatsAudioProcessor::~SquareBeatsAudioProcessor()
{
}

//==============================================================================
const juce::String SquareBeatsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SquareBeatsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SquareBeatsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SquareBeatsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SquareBeatsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SquareBeatsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SquareBeatsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SquareBeatsAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SquareBeatsAudioProcessor::getProgramName (int index)
{
    return {};
}

void SquareBeatsAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SquareBeatsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void SquareBeatsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool SquareBeatsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Accept mono or stereo output (required for Ableton compatibility)
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void SquareBeatsAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that didn't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Clear input MIDI - we only generate MIDI, don't pass through
    midiMessages.clear();
    
    // Get transport information from host using modern JUCE API
    if (auto* playHead = getPlayHead())
    {
        if (auto posInfo = playHead->getPosition())
        {
            // Extract transport state using modern optional-based API
            bool isPlaying = posInfo->getIsPlaying();
            double sampleRate = getSampleRate();
            double bpm = 120.0; // Default
            double timeInSamples = 0.0;
            double timeInBeats = 0.0;
            
            if (auto bpmOpt = posInfo->getBpm())
                bpm = *bpmOpt > 0.0 ? *bpmOpt : 120.0;
            
            if (auto samplesOpt = posInfo->getTimeInSamples())
                timeInSamples = static_cast<double>(*samplesOpt);
            
            if (auto ppqOpt = posInfo->getPpqPosition())
                timeInBeats = *ppqOpt;
            
            // Detect beat crossings for visual pulse
            if (isPlaying && timeInBeats >= 0.0)
            {
                double currentBeat = std::floor(timeInBeats);
                if (currentBeat != lastBeatPosition && lastBeatPosition >= 0.0)
                {
                    // We crossed a beat boundary
                    auto timeSignature = patternModel.getTimeSignature();
                    double beatsPerBar = timeSignature.getBeatsPerBar();
                    bool isDownbeat = (std::fmod(currentBeat, beatsPerBar) < 0.001);
                    beatPulseState.triggerBeat(isDownbeat);
                }
                lastBeatPosition = currentBeat;
            }
            else if (!isPlaying)
            {
                lastBeatPosition = -1.0;  // Reset on stop
            }
            
            // Update playback engine with transport info
            playbackEngine.handleTransportChange(isPlaying, sampleRate, bpm, 
                                                timeInSamples, timeInBeats);
        }
    }
    
    // Generate MIDI events
    playbackEngine.processBlock(buffer, midiMessages);
}

//==============================================================================
bool SquareBeatsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SquareBeatsAudioProcessor::createEditor()
{
    return new SquareBeatsAudioProcessorEditor (*this);
}

//==============================================================================
void SquareBeatsAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Serialize pattern model using StateManager
    SquareBeats::StateManager::saveState(patternModel, destData);
}

void SquareBeatsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Deserialize pattern model using StateManager
    SquareBeats::StateManager::loadState(patternModel, data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SquareBeatsAudioProcessor();
}

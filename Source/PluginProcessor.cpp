#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "cdrt/helper/Parameters.h"
#include "cdrt/utility/Conversion.h"
#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "parameters", cdrt::helper::parameters::createLayout())
{
    apvts.addParameterListener("input", this);
    apvts.addParameterListener("output", this);
    apvts.addParameterListener("time", this);
    apvts.addParameterListener("feedback", this);
    apvts.addParameterListener("dry", this);
    apvts.addParameterListener("wet", this);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    apvts.removeParameterListener("input", this);
    apvts.removeParameterListener("output", this);
    apvts.removeParameterListener("time", this);
    apvts.removeParameterListener("feedback", this);
    apvts.removeParameterListener("dry", this);
    apvts.removeParameterListener("wet", this);
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (2);
    
    // Delay lines preparation.
    for (size_t i = 0; i < numDelayLines; ++i)
    {
        delayLines.push_back(std::make_shared<cdrt::dsp::DelayLineLinear<float>>());
        delayLines[i]->prepare(spec);
        delayLines[i]->setMaxDelaySamples(maxDelayTimeInSeconds * static_cast<int> (sampleRate));
        delayLines[i]->setDelaySamples(initialDelaySamples);
        delayLines[i]->setFeedback(initialFeedback);
    }
    
    delayLineRouter = std::make_unique<cdrt::dsp::DelayLineRoutingStraight<float>>();
    delayLineRouter->prepare(delayLines);

    // Generic parameters init.
    // Reading values from apvts.
    auto inputGainParameter = apvts.getRawParameterValue("input")->load();
    auto outputGainParameter = apvts.getRawParameterValue("output")->load();

    auto delayTimeParameter = apvts.getRawParameterValue("time")->load();
    auto delayFeedbackParameter = apvts.getRawParameterValue("feedback")->load();

    auto delayDryParameter = apvts.getRawParameterValue("dry")->load();
    auto delayWetParameter = apvts.getRawParameterValue("wet")->load();

    // Apply values from apvts.
    for (auto& value: inputSmoothed)
    {
        value.reset(sampleRate, 0.25f);
        value.setTargetValue(inputGainParameter);
    }

    for (auto& value: outputSmoothed)
    {
        value.reset(sampleRate, 0.25f);
        value.setTargetValue(outputGainParameter);
    }

    for (auto& value: delayLineTimeValueSmoothed)
    {
        value.reset(sampleRate, 1.0f);
        value.setTargetValue(delayTimeParameter);
    }

    for (auto& value: delayLineFeedbackSmoothed)
    {
        value.reset(sampleRate, 0.05f);
        value.setTargetValue(delayFeedbackParameter);
    }

    for (auto& value: delayLineDrySmoothed)
    {
        value.reset(sampleRate, 0.25f);
        value.setTargetValue(delayDryParameter);
    }

    for (auto& value: delayLineWetSmoothed)
    {
        value.reset(sampleRate, 0.25f);
        value.setTargetValue(delayWetParameter);
    }
}

void AudioPluginAudioProcessor::releaseResources()
{
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono()
         && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo())
            return true;
        
        if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
         && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo())
            return true;
        
        else
            return false;
    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // copy left input to right input if input signal is mono.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.copyFrom(i, 0, buffer, 0, 0, buffer.getNumSamples());

    float *toProcessSamples = new float[2];
    float **channelDatas = new float*[2];

    float updatedInput[2];
    float updatedOutput[2];
    float updatedWet[2];
    float updatedDry[2];

    channelDatas[0] = buffer.getWritePointer(0);
    channelDatas[1] = buffer.getWritePointer(1);
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            // Read the parameter to increment for the current channel.
            updatedInput[channel] = inputSmoothed[static_cast<size_t>(channel)].getNextValue();
            updatedOutput[channel] = outputSmoothed[static_cast<size_t>(channel)].getNextValue();
            updatedWet[channel] = delayLineWetSmoothed[static_cast<size_t>(channel)].getNextValue();
            updatedDry[channel] = delayLineDrySmoothed[static_cast<size_t>(channel)].getNextValue();

            // Delay time is critical, smoothing can get it wrong sometimes and goes above the given target values.
            auto updatedTime = delayLineTimeValueSmoothed[static_cast<size_t>(channel)].getNextValue();

            // Apply the time and feedback.
            delayLines[static_cast<size_t>(channel)]->setDelayTime (updatedTime);
            delayLines[static_cast<size_t>(channel)]->setFeedback (delayLineFeedbackSmoothed[static_cast<size_t> (channel)].getNextValue());

            toProcessSamples[channel] = channelDatas[channel][sample];
        }
        toProcessSamples[0] = channelDatas[0][sample] * updatedInput[0];
        toProcessSamples[1] = channelDatas[1][sample] * updatedInput[1];

        toProcessSamples = delayLineRouter->processSamples(toProcessSamples);

        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
            channelDatas[channel][sample] = ((channelDatas[channel][sample] * updatedDry[channel]) + (updatedWet[channel] * toProcessSamples[channel])) * updatedOutput[channel];
    }
    delete [] channelDatas;
    delete [] toProcessSamples ;
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
//    return new AudioPluginAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
void AudioPluginAudioProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == "input")
    {
        for (auto& value: inputSmoothed)
        {
            value.setTargetValue(newValue);
        }
    }
    else if (parameterID == "output")
    {
        for (auto& value: outputSmoothed)
        {
            value.setTargetValue(newValue);
        }
    }
    else if (parameterID == "time")
    {
        for (auto& value: delayLineTimeValueSmoothed)
        {
            value.setTargetValue(newValue);
        }
    }
    else if (parameterID == "feedback")
    {
        for (auto& value: delayLineFeedbackSmoothed)
        {
            value.setTargetValue(newValue);
        }
    }
    else if (parameterID == "dry")
    {
        for (auto& value: delayLineDrySmoothed)
        {
            value.setTargetValue(newValue);
        }
    }
    else if (parameterID == "wet")
    {
        for (auto& value: delayLineWetSmoothed)
        {
            value.setTargetValue(newValue);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

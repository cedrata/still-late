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
    
    // Delay line preparation.
    delayLine.prepare (spec);
    
    // Temporary delay setup, must become dynaimc with parameters.
    delayLine.setMaxDelaySamples (3 * static_cast<int> (sampleRate));
    delayLine.setDelaySamples(2 * static_cast<int> (sampleRate));
    delayLine.setFeedback (0.0f);
    
    // Generic parameters init.
    // Reading values from apvts.
    auto inputGainParameter = apvts.getRawParameterValue("input")->load();
    auto outputGainParameter = apvts.getRawParameterValue("output")->load();
    
    auto delayTimeParameter = static_cast<float> (cdrt::utility::conversion::msToSamples<double> (apvts.getRawParameterValue("time")->load(), sampleRate));
    auto delayFeedbackParameter = apvts.getRawParameterValue("feedback")->load();
    
    auto delayDryParameter = apvts.getRawParameterValue("dry")->load();
    auto delayWetParameter = apvts.getRawParameterValue("wet")->load();
    
    // Apply values from apvts.
    
    // TODO: Convert to function..
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
    delayLine.reset();
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

    // const auto routing = apvts.getRawParameterValue("routing")->load();
    
    // If the input signal is mono type the single channel will be
    // copied on the second channel buffer.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
         // buffer.clear (i, 0, buffer.getNumSamples());
         buffer.copyFrom(i, 0, buffer, 0, 0, buffer.getNumSamples());

    // Plugin processing.
    float newSample;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // Get data for the current channel.
        auto* channelData = buffer.getWritePointer (channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Read the parameter to increment for the current channel.
            const auto updatedInput = inputSmoothed[static_cast<size_t>(channel)].getNextValue();
            const auto updatedOutput = outputSmoothed[static_cast<size_t>(channel)].getNextValue();
            const auto updatedWet = delayLineWetSmoothed[static_cast<size_t>(channel)].getNextValue();
            const auto updatedDry = delayLineDrySmoothed[static_cast<size_t>(channel)].getNextValue();
            
            // Apply the time and feedback for
            delayLine.setDelaySamples(delayLineTimeValueSmoothed[static_cast<size_t>(channel)].getNextValue());
            delayLine.setFeedback(delayLineFeedbackSmoothed[static_cast<size_t>(channel)].getNextValue());
            
            newSample = delayLine.processSample(channel, channelData[sample] * updatedInput);
            channelData[sample] = ((channelData[sample] * updatedDry) + (updatedWet * newSample)) * updatedOutput;
        }
    }
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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
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

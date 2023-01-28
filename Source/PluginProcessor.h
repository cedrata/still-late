#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "cdrt/dsp/DelayLine.h"
#include "cdrt/utility/Interpolation.h"


class AudioPluginAudioProcessor : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    const juce::String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // AudioProcessorValueTreeSTate::Listener
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
    // Audio parameters.
    juce::AudioProcessorValueTreeState apvts;
    
    // Delay.
    std::unique_ptr<cdrt::dsp::DelayLineBase<float>> delayLine;
    // std::vector<std::unique_ptr<cdrt::dsp::DelayLineBase<float>>, 2> dl;
    
    // Generic parameters.
    std::array<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, 2> inputSmoothed;
    std::array<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, 2> outputSmoothed;
    
    // Delay parameters.
    std::array<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, 2> delayLineTimeValueSmoothed;
    std::array<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, 2> delayLineFeedbackSmoothed;
    std::array<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, 2> delayLineDrySmoothed;
    std::array<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, 2> delayLineWetSmoothed;
};

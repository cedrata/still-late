#include "Parameters.h"

namespace cdrt
{
namespace helper
{
namespace parameters
{

juce::AudioProcessorValueTreeState::ParameterLayout createLayout(void)
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> ("input", "Input", juce::NormalisableRange<float> {0.0f, 2.0f, 0.01f}, 1.0f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> ("output", "Output", juce::NormalisableRange<float> {0.0f, 2.0f, 0.01f}, 1.0f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> ("time", "Time", juce::NormalisableRange<float> {0.0f, 3000.0f, 0.1f}, 250.0f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> ("feedback", "Feedback", juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.5f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> ("dry", "Dry", juce::NormalisableRange<float> {0.0, 1.0f, 0.01f}, 0.7f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> ("wet", "Wet", juce::NormalisableRange<float> {0.0, 1.0f, 0.01f}, 0.7f));
    parameters.push_back (std::make_unique<juce::AudioParameterChoice> ("routing", "Routing", juce::StringArray {"Straight", "Ping Pong L to R", "Ping Pong R to L"}, 0));

    return { parameters.begin(), parameters.end() };
}


} // namespace parameters
} // namespace helper
} // namespace cdrt

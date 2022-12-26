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
    
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {"input", 1}, "Input", juce::NormalisableRange<float> {0.0f, 2.0f, 0.01f}, 1.0f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {"output", 2}, "Output", juce::NormalisableRange<float> {0.0f, 2.0f, 0.01f}, 1.0f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {"time", 3}, "Time", juce::NormalisableRange<float> {0.0f, 3000.0f, 0.1f}, 250.0f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {"feedback", 4}, "Feedback", juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.5f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {"dry", 5}, "Dry", juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.7f));
    parameters.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {"wet", 6}, "Wet", juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.7f));
    parameters.push_back (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID {"routing", 7}, "Routing", juce::StringArray {"Straight", "Ping Pong L to R", "Ping Pong R to L"}, 0));

    return { parameters.begin(), parameters.end() };
}


} // namespace parameters
} // namespace helper
} // namespace cdrt

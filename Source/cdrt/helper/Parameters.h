#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace cdrt
{
namespace helper
{
namespace parameters
{
/**
 @brief: This function is a wrapper over the parameters layout creation, used to generate all the required parameters for this plugin.
 */
juce::AudioProcessorValueTreeState::ParameterLayout createLayout(void);

} // namespace parameters
} // namespace helper
} // namespace cdrt

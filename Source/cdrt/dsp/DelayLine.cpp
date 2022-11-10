#include "DelayLine.h"
#include "../utility/Conversion.h"

namespace cdrt
{
namespace dsp
{
//==============================================================================
// Public.

//==============================================================================
// Constructor.

template <typename SampleType, typename  InterpolationType>
DelayLine <SampleType, InterpolationType>::DelayLine (const int newMaxBufferSize)
{
    setMaxDelaySamples(newMaxBufferSize);
}

//==============================================================================
// Allocation/Deallocation.

template <typename SampleType, typename  InterpolationType>
void DelayLine <SampleType, InterpolationType>::prepare (const juce::dsp::ProcessSpec& spec)
{
    jassert (spec.numChannels > 0);

    buffer.setSize (static_cast<int> (spec.numChannels), maxBufferSize, false, false, true);

    writePointer.resize (spec.numChannels);
    readPointer.resize (spec.numChannels);

    prev.resize (spec.numChannels);
    sampleRate = spec.sampleRate;

    reset();
}

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::reset()
{
    for (auto vec: { &writePointer, &readPointer })
        std::fill (vec->begin(), vec->end(), 0);

    std::fill (prev.begin(), prev.end(), static_cast<SampleType> (0));

    buffer.clear();
}

//==============================================================================
// Setters.

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::setMaxDelaySamples (const int newMaxBufferSize)
{
    jassert (newMaxBufferSize > 0);

    maxBufferSize = newMaxBufferSize;
    buffer.setSize (static_cast<int>(numChannels), maxBufferSize, false, false, true);
}

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::setDelaySamples (const float newDelaySamples)
{
    jassert (juce::isPositiveAndBelow (newDelaySamples, maxBufferSize));

    delaySamples = newDelaySamples;
    delayInt = static_cast<int> (std::floor (delaySamples));
    delayFrac = delaySamples - delayInt;

    updateInternalVariables();
}

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::setDelayTime (const float delayTime)
{
    auto newDelaySamples = cdrt::utility::conversion::msToSamples<float>(delayTime, static_cast<float>(sampleRate));
    setDelaySamples (newDelaySamples);
}

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::setFeedback (const float newFeedback)
{
    feedback = newFeedback;
}

//==============================================================================
// Getters.

template <typename SampleType, typename InterpolationType>
int DelayLine <SampleType, InterpolationType>::getMaximumDelaySamples() const noexcept
{
    return maxBufferSize - 1;
}

//==============================================================================
// Processing.

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::pushSample (const int channel, const SampleType sample)
{
    jassert (juce::isPositiveAndBelow (channel, numChannels));

    buffer.setSample (channel, writePointer[static_cast<size_t>(channel)], sample);
    writePointer[static_cast<size_t> (channel)] = (writePointer[static_cast<size_t> (channel)] + 1) % maxBufferSize;
}

template <typename SampleType, typename InterpolationType>
SampleType DelayLine <SampleType, InterpolationType>::popSample (const int channel)
{
    jassert (juce::isPositiveAndBelow (channel, numChannels));

    // Read pos is used paired with the delayInt value.
    auto result = interpolateSample (channel);
    readPointer[static_cast<size_t> (channel)] = (readPointer[static_cast<size_t> (channel)] + 1) % maxBufferSize;

    return result;
}

template <typename SampleType, typename InterpolationType>
SampleType DelayLine <SampleType, InterpolationType>::processSample (const int channel, const float sample)
{
    pushSample (channel, sample);

    return popSample (channel);
}

//==============================================================================
// Specialization.
template class DelayLine<float, utility::interpolation::InterpolationTypes::None>;
template class DelayLine<double, utility::interpolation::InterpolationTypes::None>;
template class DelayLine<float, utility::interpolation::InterpolationTypes::Linear>;
template class DelayLine<double, utility::interpolation::InterpolationTypes::Linear>;
template class DelayLine<float, utility::interpolation::InterpolationTypes::Lagrange3rd>;
template class DelayLine<double, utility::interpolation::InterpolationTypes::Lagrange3rd>;
template class DelayLine<float, utility::interpolation::InterpolationTypes::Thiran>;
template class DelayLine<double, utility::interpolation::InterpolationTypes::Thiran>;

} // namespace dsp
} // namespace cdrt

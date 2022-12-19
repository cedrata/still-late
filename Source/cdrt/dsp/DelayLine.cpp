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
DelayLine <SampleType, InterpolationType>::DelayLine ()
{
    setMaxDelaySamples(0);
}

//==============================================================================
// Allocation/Deallocation.

template <typename SampleType, typename  InterpolationType>
void DelayLine <SampleType, InterpolationType>::prepare (const juce::dsp::ProcessSpec& spec)
{
    jassert (spec.numChannels > 0);
    numChannels = spec.numChannels;

    buffer.setSize (static_cast<int> (numChannels), maxBufferSize, false, false, true);

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
    jassert (newMaxBufferSize >= 0);

    maxBufferSize = newMaxBufferSize;
    buffer.setSize (buffer.getNumChannels(), maxBufferSize, false, false, true);
}

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::setDelaySamples (const float newDelaySamples)
{
    jassert (juce::isPositiveAndBelow (newDelaySamples, maxBufferSize));
    
    auto toIncrement = static_cast<int>(std::trunc (delaySamples - newDelaySamples));
    for (auto &i: readPointer)
    {
        i += toIncrement;
        i = (i % maxBufferSize + maxBufferSize) % maxBufferSize;
    }

    delaySamples = newDelaySamples;
    delayInt = static_cast<int> (std::trunc (delaySamples));
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

template <typename SampleType, typename InterpolationType>
SampleType DelayLine <SampleType, InterpolationType>::getSample(const int channel, const int index) const
{
    return buffer.getSample(channel, index);
}

//==============================================================================
// Processing.

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::putSample (const int channel, const SampleType sample)
{
    jassert (juce::isPositiveAndBelow (channel, numChannels));
    
    auto interpolation = interpolateSample(channel);
    auto toWriteSample = sample + interpolation * feedback;
    
    buffer.setSample (channel, writePointer[static_cast<size_t>(channel)], toWriteSample);
    writePointer[static_cast<size_t> (channel)] = (writePointer[static_cast<size_t> (channel)] + 1) % maxBufferSize;
}

template <typename SampleType, typename InterpolationType>
SampleType DelayLine <SampleType, InterpolationType>::popSample (const int channel)
{
    jassert (juce::isPositiveAndBelow (channel, numChannels));

    // Read pos is used paired with the delayInt value.
//    auto result = interpolateSample<InterpolationType> (channel);
    auto result = buffer.getSample(channel, readPointer[static_cast<size_t> (channel)]);
    readPointer[static_cast<size_t> (channel)] = (readPointer[static_cast<size_t> (channel)] + 1) % maxBufferSize;

    return result;
}

template <typename SampleType, typename InterpolationType>
SampleType DelayLine <SampleType, InterpolationType>::processSample (const int channel, const float sample)
{
    putSample (channel, sample);
    auto result = popSample (channel);
    return result;
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

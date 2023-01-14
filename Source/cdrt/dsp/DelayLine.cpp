#include "./DelayLine.h"
#include "../utility/Conversion.h"

namespace cdrt
{
namespace dsp
{
//==============================================================================
// class DelayLineBase

//==============================================================================
// Public.

//==============================================================================
// Constructor.

template<typename SampleType>
DelayLineBase<SampleType>::DelayLineBase()
{
    setMaxDelaySamples(0);
}

//==============================================================================
// Allocation/Deallocation.

template<typename SampleType>
void DelayLineBase<SampleType>::prepare (const juce::dsp::ProcessSpec& spec)
{
    jassert (spec.numChannels > 0);
    numChannels = spec.numChannels;

    buffer.setSize (static_cast<int> (numChannels), maxBufferSize, false, false, true);

    writePointer.resize (spec.numChannels);
    readPointer.resize (spec.numChannels);

    // prev.resize (spec.numChannels); // add inside Thiran interpolationn delay.
    sampleRate = spec.sampleRate;

    reset();
}

template<typename SampleType>
void DelayLineBase<SampleType>::reset()
{
    for (auto vec: { &writePointer, &readPointer })
        std::fill (vec->begin(), vec->end(), 0);

    // std::fill (prev.begin(), prev.end(), static_cast<SampleType> (0)); // add inside Thiran interpolation delay.

    buffer.clear();
}

//==============================================================================
// Setters.

template<typename SampleType>
void DelayLineBase<SampleType>::setMaxDelaySamples (const int newMaxBufferSize)
{
    jassert (newMaxBufferSize >= 0);

    maxBufferSize = newMaxBufferSize;
    buffer.setSize (buffer.getNumChannels(), maxBufferSize, false, false, true);
}

template<typename SampleType>
void DelayLineBase<SampleType>::setDelaySamples (const float newDelaySamples)
{
    jassert (juce::isPositiveAndNotGreaterThan (newDelaySamples, maxBufferSize));

    delaySamples = newDelaySamples;
    delayInt = static_cast<int> (std::floor (delaySamples));
    delayFrac = delaySamples - delayInt;
    
    updateInternalVariables();
}

template<typename SampleType>
void DelayLineBase<SampleType>::setDelayTime (const float delayTime)
{
    auto newDelaySamples = cdrt::utility::conversion::msToSamples<float>(delayTime, static_cast<float>(sampleRate));
    setDelaySamples (newDelaySamples);
}

template<typename SampleType>
void DelayLineBase<SampleType>::setFeedback (const float newFeedback)
{
    feedback = newFeedback;
}

//==============================================================================
// Getters.

template <typename SampleType>
int DelayLineBase<SampleType>::getMaximumDelaySamples() const noexcept
{
    return maxBufferSize;
}

template <typename SampleType>
SampleType DelayLineBase<SampleType>::getSample (const int channel, const int index) const
{
    return buffer.getSample(channel, index);
}

template <typename SampleType>
int DelayLineBase<SampleType>::getReadIndex(const int channel) const
{
    return ((readPointer[static_cast<size_t> (channel)] - delayInt) % getMaximumDelaySamples() + getMaximumDelaySamples()) % getMaximumDelaySamples();
}

//==============================================================================
// Processing.

template <typename SampleType>
void DelayLineBase<SampleType>::putSample (const int channel, const SampleType sample)
{
    jassert (juce::isPositiveAndBelow (channel, numChannels));
    
    auto interpolation = interpolateSample(channel);
    auto toWriteSample = sample + interpolation * feedback;
    
    buffer.setSample (channel, writePointer[static_cast<size_t> (channel)], toWriteSample);
    writePointer[static_cast<size_t> (channel)] = (writePointer[static_cast<size_t> (channel)] + 1) % getMaximumDelaySamples();
}

template <typename SampleType>
SampleType DelayLineBase<SampleType>::popSample (const int channel, const bool updatePointer)
{
    jassert (juce::isPositiveAndBelow (channel, numChannels));

    // Calculate the delayed delay index.
    // This calulation is required because it will calculate the module of negative values.
    const auto readIndex = ((readPointer[static_cast<size_t> (channel)] - delayInt) % getMaximumDelaySamples() + getMaximumDelaySamples()) % getMaximumDelaySamples();
    auto result = buffer.getSample(channel, readIndex);
    
    // Baranchelss code of:
    // if (updatePointer)
    // {
    //     readPointer[static_cast<size_t> (channel)] = readPointer[static_cast<size_t> (channel)] + 1) % getMaximumDelaySamples();
    // }
    readPointer[static_cast<size_t> (channel)] = (updatePointer * ((readPointer[static_cast<size_t> (channel)] + 1) % getMaximumDelaySamples())) + (!updatePointer * readPointer[static_cast<size_t> (channel)]);

    return result;
}

template <typename SampleType>
SampleType DelayLineBase<SampleType>::processSample (const int channel, const float sample)
{
    putSample (channel, sample);
    auto result = popSample (channel);
    return result;
}

//===============================================================================
// class DelayLineNone
template <typename SampleType>
SampleType DelayLineNone<SampleType>::interpolateSample (const int channel)
{
    // Retriving index to read from.
    const auto index = DelayLineBase<SampleType>::getReadIndex (channel);

    // Retriving the sample from the index retrived in previous step.
    return this->buffer.getSample(channel, index);
}
} // namespace dsp
} // namespace cdrt

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

    sampleRate = spec.sampleRate;

    reset();
}

template<typename SampleType>
void DelayLineBase<SampleType>::reset()
{
    for (auto vec: { &writePointer, &readPointer })
        std::fill (vec->begin(), vec->end(), 0);

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

template <typename SampleType>
juce::uint32 DelayLineBase<SampleType>::getMaxBlocks() const
{
    return maxBlocks;
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

template class DelayLineBase<float>;
template class DelayLineBase<double>;


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

template class DelayLineNone<float>;
template class DelayLineNone<double>;


//===============================================================================
// class DelayLineLinear
template <typename SampleType>
SampleType DelayLineLinear<SampleType>::interpolateSample (const int channel)
{
    // Retriving index to read from.
    auto index1 = this->writePointer[static_cast<size_t> (channel)];
    auto index2 = (index1 + 1) % this->maxBufferSize;
    
    // Retriving samples from indexes retrived in previous step.
    auto sample1 = this->buffer.getSample(channel, index1);
    auto sample2 = this->buffer.getSample(channel, index2);
    
    return cdrt::utility::interpolation::linear<SampleType>(sample1, sample2, this->delayFrac);
    updateInternalVariables();
}

template class DelayLineLinear<float>;
template class DelayLineLinear<double>;


//===============================================================================
// class DelayLineLagrange3rd
template <typename SampleType>
SampleType DelayLineLagrange3rd<SampleType>::interpolateSample (const int channel)
{
    // Retriving index to read from.
    auto index1 = this->writePointer[static_cast<size_t> (channel)];
    auto index2 = (index1 + 1) % this->maxBufferSize;
    auto index3 = (index2 + 1) % this->maxBufferSize;
    auto index4 = (index3 + 1) % this->maxBufferSize;
    
    // Retriving samples from indexes retrived in previous step.
    auto sample1 = this->buffer.getSample(channel, index1);
    auto sample2 = this->buffer.getSample(channel, index2);
    auto sample3 = this->buffer.getSample(channel, index3);
    auto sample4 = this->buffer.getSample(channel, index4);
    
    auto res = cdrt::utility::interpolation::lagrange3rd<SampleType>(sample1, sample2, sample3, sample4, this->delayFrac);
    
    updateInternalVariables();
        
    return res;
}

template <typename SampleType>
void DelayLineLagrange3rd<SampleType>::updateInternalVariables()
{
    if (this->delayInt >= 1)
    {
        this->delayFrac++;
        this->delayInt--;
    }
}

template class DelayLineLagrange3rd<float>;
template class DelayLineLagrange3rd<double>;


//===============================================================================
// class DelayLineThiran

// Allocation/Deallocation.
template <typename SampleType>
void DelayLineThiran<SampleType>::prepare (const juce::dsp::ProcessSpec &spec)
{
    jassert (spec.numChannels > 0);
    this->numChannels = spec.numChannels;

    this->buffer.setSize (static_cast<int> (this->numChannels), this->maxBufferSize, false, false, true);

    this->writePointer.resize (spec.numChannels);
    this->readPointer.resize (spec.numChannels);

    prev.resize (spec.numChannels);
    this->sampleRate = spec.sampleRate;

    reset();
}


template <typename SampleType>
void DelayLineThiran<SampleType>::reset()
{
    for (auto vec: { &this->writePointer, &this->readPointer })
        std::fill (vec->begin(), vec->end(), 0);

     std::fill (prev.begin(), prev.end(), static_cast<SampleType> (0));

    this->buffer.clear();
}

// Processing
template <typename SampleType>
SampleType DelayLineThiran<SampleType>::interpolateSample (const int channel)
{
    // Retriving index to read from.
    auto index1 = this->writePointer[static_cast<size_t> (channel)];
    auto index2 = (index1 + 1) % this->maxBufferSize;
    
    // Retriving samples from indexes retrived in previous step.
    auto sample1 = this->buffer.getSample(channel, index1);
    auto sample2 = this->buffer.getSample(channel, index2);
    
    // Calculating result and updating prev[channel] variable.
    auto result = cdrt::utility::interpolation::thiran<SampleType>(sample1, sample2, this->delayFrac, alpha, prev[static_cast<size_t> (channel)]);
    
    this->prev[static_cast<size_t>(channel)] = result;
    
    updateInternalVariables();
    
    return result;
}

template <typename SampleType>
void DelayLineThiran<SampleType>::updateInternalVariables()
{
    if (this->delayFrac < (SampleType) 0.618 && this->delayInt >= 1)
    {
        this->delayFrac++;
        this->delayInt--;
    }

    alpha = (1 - this->delayFrac) / (1 + this->delayFrac);
}

template class DelayLineThiran<float>;
template class DelayLineThiran<double>;
} // namespace dsp
} // namespace cdrt

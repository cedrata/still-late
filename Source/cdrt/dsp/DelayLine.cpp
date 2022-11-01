#include "DelayLine.h"

namespace cdrt
{
namespace dsp
{
//==============================================================================
// Public.

//==============================================================================
// Constructor.

template <typename SampleType, typename  InterpolationType>
DelayLine <SampleType, InterpolationType>::DelayLine (const int maxBufferSize)
{
    // TODO: Implement.
    // jassert (maxBufferSize >= 0);
    // this->maxBufferSize = maxBufferSize;
}

//==============================================================================
// Allocation/Deallocation.

template <typename SampleType, typename  InterpolationType>
void DelayLine <SampleType, InterpolationType>::prepare (const juce::dsp::ProcessSpec& spec, bool singleChannel)
{
    // jassert (spec.numChannels > 0)

    // spec represent sistem specs
    // check singleChannel, is it necessary? Yes or no? It actually should be to input from stereo to mono.

    // TODO: Implement.
}

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::reset()
{
    // TODO: Implement.
}

//==============================================================================
// Setters.

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::setDelaySamples (const SampleType &delaySamples)
{
    // TODO: Implement.
}

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::setDelayTime (const SampleType &delayTime)
{
    // TODO: Implement.
}

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::setFeedback (const float &feedback)
{
    // TODO: Implement.
}

//==============================================================================
// Processing.

template <typename SampleType, typename InterpolationType>
void DelayLine <SampleType, InterpolationType>::putSample (const SampleType& sample)
{
    // TODO: Implement.
}

template <typename SampleType, typename InterpolationType>
SampleType DelayLine <SampleType, InterpolationType>::popSample()
{
    // TODO: Implement.
    return 0.0;
}

template <typename SampleType, typename InterpolationType>
SampleType DelayLine <SampleType, InterpolationType>::processSample (const int& channel, const float& inputSample)
{
    // TODO: Implement.
    return 0.0;
}


} // namespace dsp
} // namespace cdrt
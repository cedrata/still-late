#pragma once

#include <type_traits>
#include <vector>

#include "../dsp/DelayLine.h"

namespace cdrt
{
namespace utility
{
namespace routing
{

//==============================================================================
// A collection of structs to use as the tempalte argument whie performing some
// some kind of audio routing samples transformation.
namespace RoutingTypes
{
    // Struct to use when handling any kind of Mono to Stereo audio signal.
    struct MonoToStereo {};

    // Struct to use when handling any kind of Stero to Stereo audio signal.
    struct StereoToStereo {};
} // namespace RoutingTypes

/**
 * @brief: Call this function when Straight Delay Line routing is required to handle samples processing
 * with Mono to Stereo routing.
 * This funciton isn't meant to be from other places other than the DelayLineRoutingStraight class.
 * Be careful using this class because no checks are made about data validity in here.
 * @param samples: vector containing input samples that must be pushed to the DelayLine buffers.
 * @param delayLines: vector containing weak pointers pointing to the DelayLine instances.
 * @return std::vector<SampleType>
 */
template <typename SampleType, typename RoutingType, std::enable_if_t<std::is_same_v<RoutingType, RoutingTypes::MonoToStereo> && std::is_floating_point_v<SampleType>, bool> = true>
SampleType** processDelayLineRoutingStraight(SampleType** samples, std::vector<std::weak_ptr<dsp::DelayLineBase<SampleType>>> delayLines)
{
    // If the execution gets here it means:
    // - ONE input buffer only is populated with meaningful values
    // - TWO outputs must be returned (which is the provided input
    // - This code assume the provided shared pointers are valid
    // - This code assume to recive in input 2 delay lines pointers
    // - This cose assume each delay line has one channel only
    
    auto dl1 = delayLines[0].lock();
    auto dl2 = delayLines[1].lock();
    
    auto maxBlocks = dl1->getMaxBlocks();
    
    SampleType toProcessSample;
    for (size_t i = 0; i < static_cast<size_t>(maxBlocks); ++i)
    {
        toProcessSample = samples[0][i];
        samples[0][i] = dl1->processSample(0, toProcessSample);
        samples[1][i] = dl2->processSample(0, toProcessSample); // Get samples from firs
    }
    
    return samples;
}

/**
 * @brief: Call this function when Straight Delay Line routing is required to handle samples processing
 * with Stereo to Stereo routing.
 * This funciton isn't meant to be from other places other than the DelayLineRoutingStraight class.
 * Be careful using this class because no checks are made about data validity in here.
 * @param samples: vector containing input samples that must be pushed to the DelayLine buffers.
 * @param delayLines: vector containing weak pointers pointing to the DelayLine instances.
 * @return std::vector<SampleType>
 */
template <typename SampleType, typename RoutingType, std::enable_if_t<std::is_same_v<RoutingType, RoutingTypes::StereoToStereo> && std::is_floating_point_v<SampleType>, bool> = true>
SampleType** processDelayLineRoutingStraight(SampleType** samples, std::vector<std::weak_ptr<dsp::DelayLineBase<SampleType>>> delayLines)
{
    // If the execution gets here it means:
    // - TWO input buffer only is populated with meaningful values
    // - TWO outputs must be returned (which is the provided input
    // - This code assume the provided shared pointers are valid
    // - This code assume to recive in input 2 delay lines pointers
    // - This cose assume each delay line has one channel only
    
    auto dl1 = delayLines[0].lock();
    auto dl2 = delayLines[1].lock();
    
    auto maxBlocks = dl1->getMaxBlocks();
    
    for (size_t i = 0; i < static_cast<size_t>(maxBlocks); ++i)
    {
        samples[0][i] = dl1->processSample(0, samples[0][i]);
        samples[1][i] = dl2->processSample(0, samples[1][i]);
    }
    
    return samples;
}

///**
// * @brief: Call this function when PingPong Delay Line routing is required to handle samples processing
// * with Mono to Stereo routing.
// * This funciton isn't meant to be from other places other than the DelayLineRoutingStraight class.
// * @param samples: vector containing input samples that must be pushed to the DelayLine buffers.
// * @param delayLines: vector containing weak pointers pointing to the DelayLine instances.
// * @return std::vector<SampleType>
// */
//template <typename SampleType, typename RoutingType, std::enable_if_t<std::is_same_v<RoutingType, RoutingTypes::MonoToStereo> && std::is_floating_point_v<SampleType>, bool> = true>
//std::vector<SampleType> processDelayLineRoutingPingPong(std::vector<SampleType> samples, std::vector<std::weak_ptr<dsp::DelayLineBase<SampleType>>> delayLines)
//{
//    // TODO: implementation...
//    delayLines.size();
//    return samples;
//}
//
///**
// * @brief: Call this function when Straight Delay Line routing is required to handle samples processing
// * with Stereo to Stereo routing.
// * This funciton isn't meant to be from other places other than the DelayLineRoutingStraight class.
// * @param samples: vector containing input samples that must be pushed to the DelayLine buffers.
// * @param delayLines: vector containing weak pointers pointing to the DelayLine instances.
// * @return std::vector<SampleType>
// */
//template <typename SampleType, typename RoutingType, std::enable_if_t<std::is_same<RoutingType, RoutingTypes::StereoToStereo>::value && std::is_floating_point<SampleType>::value, bool> = true>
//std::vector<SampleType> processDelayLineRoutingPingPong(std::vector<SampleType> samples, std::vector<std::weak_ptr<dsp::DelayLineBase<SampleType>>> delayLines)
//{
//    // TODO: implementation...
//    delayLines.size();
//    return samples;
//}

} // namespace routing
} // namespace utility
} // namespace cdrt

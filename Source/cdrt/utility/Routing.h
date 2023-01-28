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
 * @param samples: vector containing input samples that must be pushed to the DelayLine buffers.
 * @param delayLines: vector containing weak pointers pointing to the DelayLine instances.
 * @return std::vector<SampleType>
 */
template <typename SampleType, typename RoutingType, std::enable_if_t<std::is_same_v<RoutingType, RoutingTypes::MonoToStereo> && std::is_floating_point_v<SampleType>, bool> = true>
std::vector<SampleType> processDelayLineRoutingStraight(std::vector<SampleType> samples, std::vector<std::weak_ptr<dsp::DelayLineBase<SampleType>>> delayLines)
{
    // TODO: implementation...
    delayLines.size();
    return samples;
}

/**
 * @brief: Call this function when Straight Delay Line routing is required to handle samples processing
 * with Stereo to Stereo routing.
 * This funciton isn't meant to be from other places other than the DelayLineRoutingStraight class.
 * @param samples: vector containing input samples that must be pushed to the DelayLine buffers.
 * @param delayLines: vector containing weak pointers pointing to the DelayLine instances.
 * @return std::vector<SampleType>
 */
template <typename SampleType, typename RoutingType, std::enable_if_t<std::is_same<RoutingType, RoutingTypes::StereoToStereo>::value && std::is_floating_point<SampleType>::value, bool> = true>
std::vector<SampleType> processDelayLineRoutingStraight(std::vector<SampleType> samples, std::vector<std::weak_ptr<dsp::DelayLineBase<SampleType>>> delayLines)
{
    // TODO: implementation...
    delayLines.size();
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

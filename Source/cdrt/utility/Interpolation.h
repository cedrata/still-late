#pragma once

#include <type_traits>

namespace cdrt
{
namespace utility
{
namespace interpolation
{

//==============================================================================
// A collection of structs as to use as the template argument when setting the
// DelayLine interpolation type.
namespace InterpolationTypes
{
    // No interpolation.
    struct None {};
    
    // Successive samples in the delay will be interpolated using linear
    // linear interpolation. Low computational cost, optimal for real-time
    // modulation with a downside: this one introduces a low-pass filering
    // effect in the processed signal.
    struct Linear {};

    // Successive samples in the delay line will be interpolated using a 3rd order
    // Lagrange interpolator. This method incurs more computational overhead than
    // linear interpolation but reduces the low-pass fiter. Real-time modulation
    // still possible but heavier computation.
    struct Lagrange3rd {};

    // Successive samples in the delay will be interpolated using 1st order
    // Thiran interpolation. Very efficient with flat amplitude frequency response
    // but with less accuracy in the phase response.
    // This interpolation is stateful, means it is unsuitable for applications
    // requiring fast delay modulation.
    struct Thiran {};
} // namespace InterpolationTypes.

//==============================================================================
// Type limitation with std::static_assert and std::is_floating_point...
// static assert is good because it runs at compile time.

// At the moment not implemented because it would be useless...
// No interpolation function.
//template <typename SampleType>
//SampleType noInterpolation (void);

// Linear interpolation function.
template <typename SampleType, std::enable_if_t<std::is_floating_point<SampleType>::value, bool> = true>
SampleType linear (const SampleType sample1, const SampleType sample2, const float delayFrac)
{
    return sample1 + delayFrac * (sample2 - sample1);
}

// Lagrange interpolation function.
template <typename SampleType, std::enable_if_t<std::is_floating_point<SampleType>::value, bool> = true>
SampleType lagrange3rd (const SampleType sample1, const SampleType sample2, const SampleType sample3, const SampleType &sample4, const float delayFrac)
{
    float d1 = delayFrac - 1.f;
    float d2 = delayFrac - 2.f;
    float d3 = delayFrac - 3.f;
    
    auto c1 = -d1 * d2 * d3 / 6.f;
    auto c2 = d2 * d3 * 0.5f;
    auto c3 = -d1 * d3 * 0.5f;
    auto c4 = d1 * d2 / 6.f;
    
    return sample1 * c1 + delayFrac * (sample2 * c2 + sample3 * c3 + sample4 * c4);
}

// Thiran interpolation function.
template <typename SampleType>
SampleType thiran (const SampleType sample1, const SampleType sample2, const float delayFrac, const SampleType alpha, const SampleType &prev)
{
    // Branched version (slower execution):
    // if (delayFrac == 0) return sample1;
    // return sample2 + alpha * (sample1 - prev);
    
    // Branchless version (faster execution):
    return (delayFrac == 0) * sample1 + (delayFrac != 0) * (sample2 + alpha * (sample1 - prev));
}

} // namespace interpolation
} // namespace utility
} // namespace cdrt


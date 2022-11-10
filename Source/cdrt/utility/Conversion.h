#pragma once

#include <type_traits>

namespace cdrt
{
namespace utility
{
namespace conversion
{

//==============================================================================
// General purpose conversion functions.

// Convert milliseconds to samples given the samplerate.
// double msToSamples(const double time, const double sampleRate);
template <typename SampleType, std::enable_if_t <std::is_floating_point <SampleType>::value, bool> = true>
SampleType msToSamples (const SampleType time, const SampleType samplerate)
{
    return static_cast<SampleType>(time * samplerate / 1000.0);
}

} // namespace conversion
} // namespace utility
} // namespace cdrt

#pragma once

namespace cdrt
{
namespace utility
{
namespace conversion
{

//==============================================================================
// General purpose conversion functions.

// Convert milliseconds to samples given the samplerate.
double msToSamples(const double time, const double sampleRate);


} // namespace conversion
} // namespace utility
} // namespace cdrt

#include "Conversion.h"

namespace cdrt
{
namespace utility
{
namespace conversion
{

double msToSamples(const double time, const double sampleRate)
{
    return time * sampleRate / 1000.0;
}

} // namespace conversion
} // namespace utility
} // namespace cdrt

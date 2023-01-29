#include "./DelayLineRouting.h"

namespace cdrt
{
namespace dsp
{

template <typename SampleType>
void DelayLineRoutingBase<SampleType>::prepare(std::vector<std::shared_ptr<cdrt::dsp::DelayLineBase<SampleType>>> newDelayLines) noexcept
{
    // Before doing anything release old shared pointers.
    reset();
    
    for (auto &dl: newDelayLines)
    {
        delayLines.push_back(dl);
    }
}

template <typename SampleType>
void DelayLineRoutingBase<SampleType>::reset() noexcept
{
    // Erase all references to old pointers.
    // It is not required to check if the pointer is expired or not.
    // If this method is called that means the prepare method was called
    // and if it has been called a new vector of delay lines has been passed
    // to be stored as vector of weak pointers here, this means we do not care
    // about old pointers. They the old shared pointers must be handled
    // outside this class.
    delayLines.erase(delayLines.begin());
}

template class DelayLineRoutingBase<float>;
template class DelayLineRoutingBase<double>;

template <typename SampleType, typename RoutingType>
SampleType** DelayLineRoutingStraight<SampleType, RoutingType>::processSamples(SampleType** samples)
{
    return  cdrt::utility::routing::processDelayLineRoutingStraight<SampleType, RoutingType>(samples, this->delayLines);
}

template class DelayLineRoutingStraight<float, utility::routing::RoutingTypes::MonoToStereo>;
template class DelayLineRoutingStraight<float, utility::routing::RoutingTypes::StereoToStereo>;
template class DelayLineRoutingStraight<double, utility::routing::RoutingTypes::MonoToStereo>;
template class DelayLineRoutingStraight<double, utility::routing::RoutingTypes::StereoToStereo>;
} // namespace dsp
} // namespace cdrt


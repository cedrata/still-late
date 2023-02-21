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
//    delayLines.erase(delayLines.begin(), delayLines.end());
}

template class DelayLineRoutingBase<float>;
template class DelayLineRoutingBase<double>;

template <typename SampleType>
SampleType* DelayLineRoutingStraightMonoToStereo<SampleType>::processSamples(SampleType* samples)
{
    {
        auto channel0 = this->delayLines[0].lock();
        auto channel1 = this->delayLines[1].lock();
        auto temp = samples[0];
        
        samples[0] = channel0->processSample(0, temp);
        samples[1] = channel1->processSample(0, temp);
    }
    
    return samples;
}

template class DelayLineRoutingStraightMonoToStereo<float>;
template class DelayLineRoutingStraightMonoToStereo<double>;
} // namespace dsp
} // namespace cdrt


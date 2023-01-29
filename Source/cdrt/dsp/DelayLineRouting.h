#pragma once

#include "./DelayLine.h"
#include "../utility/Routing.h"
namespace cdrt
{
namespace dsp
{

template <typename SampleType>
class DelayLineRoutingBase
{
public:
    //==========================================================================
    // Default constructor.
    
    /**
     * @brief Construct a new DelayLineRouting object.
     */
    DelayLineRoutingBase() {}
    
    
    //==========================================================================
    // Destructor.
    
    /**
     * Because it's an abstract class the destructor must be implemented in the derived class.
     */
    virtual ~DelayLineRoutingBase() = default;
    
    //==========================================================================
    // Allocation/Deallocation.
    
    /**
     * @brief: This method will prepare the delay line router by accepting informations about channels and the delay line that has to be used.
     * @param newInputChannels: is the number of input channels to be used.
     * @param newOutputChannels: is the number of output channels to be used.
     * @param newDelayLinesPtr: is a reference to the pointers of the dleya line to use.
     */
    virtual void prepare(std::vector<std::shared_ptr<cdrt::dsp::DelayLineBase<SampleType>>> newDelayLines) noexcept;
    
    /**
     * @brief: This method will reset the provided pointer releasing their resources leaving room for new delay lines pointers.
     */
    virtual void reset() noexcept;
    
    //==========================================================================
    // Processing.
    
    /**
     * @brief This method will return the output samples from you DelayLine instances and push the push
     * the given samples into the DelayLine instances given the selected routing method.
     *
     * @param samples: input samples to feed the DelayLine instances.
     * @return std::vector<SampleType>
     */
    virtual SampleType** processSamples(SampleType** samples) = 0;
    
protected:
    std::vector<std::weak_ptr<cdrt::dsp::DelayLineBase<SampleType>>> delayLines;
}; // class DelayLineRoutingBase

template <typename SampleType, typename RoutingType>
class DelayLineRoutingStraight: public DelayLineRoutingBase<SampleType>
{
public:
    //==========================================================================
    // Destructor.
    
    /**
     * DelayLineRoutingStraight destructor.
     */
    ~DelayLineRoutingStraight () override {}

    //==========================================================================
    // Processing.
    
    SampleType** processSamples(SampleType** samples) override;
}; // class DelayLineStraight

} // namespace dsp
} // namespace cdrt

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>
#include "../utility/Interpolation.h"

namespace cdrt
{
namespace dsp
{

// Base class for delay line w/ polymorphism.
template <typename SampleType>
class DelayLineBase
{
public:
    //==========================================================================
    // Default constructor.
    
    /**
     * @brief Construct a new DelayLine object.
     * @param newMaxBufferSize: maximum acceptable size of the buffer, upper limit to max delay time.
     */
    DelayLineBase();
    
    
    //==========================================================================
    // Destructor.
    
    /**
     * Because it's an abstract class the destructor must be implemented in the derived classes.
     */
    virtual ~DelayLineBase() = default;
    
    
    //==========================================================================
    // Allocation/Deallocation.

    /**
     * @brief Call this method before doing anything else to initialize the processor.
     *
     * @param spec: context informations for processor.
     */
    void prepare (const juce::dsp::ProcessSpec& spec);

    /**
     * @brief This method initializes the members conserving a stae of the delay like the circular buffer.
     *
     */
    void reset();
    
    //==========================================================================
    // Setters.

    /**
     * @brief This method sets the maxDelaySamples number.
     *
     * @param maxBufferSize: maximum acceptable size of the buffer, upper limit to max the delay time.
     */
    void setMaxDelaySamples (const int newMaxBufferSize);

    /**
     * @brief This method sets the delay time given a length expressed in samples.
     *
     * @param delaySamples: delay expressed in samples.
     */
    void setDelaySamples (const float newDelaySamples);

    /**
     * @brief This methods sets the delay time given a length expressed in milliseconds.
     *
     * @param delayTime: delay expressed in milliseconds.
     */
    void setDelayTime (const float delayTime);

    /**
     * @brief This method sets the amount of feedback for the delay line.
     *
     * @param feedback
     */
    void setFeedback (const float newFeedback);

    //==========================================================================
    // Getters.

    /**
     * @brief This method gets the maximum delay in samples.
     *
     * @return int
     */
    int getMaximumDelaySamples() const noexcept;
    
    /**
     * @brief This method gets the value stored at a precise index and channel in the circular buffer without checks on the index value. This method exist for testing purposes but can be used as you prefer if you are creative enough :)
     * Thi method will not interact with the values of read and write pointer, so you can read the same value many times without altering the buffer.
     *
     * @param channel: chanel where to get the sample from.
     * @param index: value of the index to get the value from.
     * @return SampleType
     *
     */
    SampleType getSample (const int channel, const int index) const;

    /**
     * @brief This method get the index where to read at given the setted delay in sapmles.
     *
     * @param channel: channel where to get the read index from.
     * @return int
     */
    int getReadIndex(const int channel) const;
    
    //==========================================================================
    // Processing.

    /**
     * @brief This method puts a sample into the selected channel.
     *
     * @param sample: value to store in circular buffer.
     * @param channel: channel where to store the sample.
     */
    void putSample (const int channel, const SampleType sample);

    /**
     * @brief This method pops a sample from the given channel.
     *
     * @param channel: channel where to get the sample from.
     * @param updatePointer: if you want to read again the same sample from the buffer set this to false.
     * @return SampleType
     */
    SampleType popSample (const int channel, const bool updatePointer = true);

    /**
     * @brief TODO: Descritpion.
     *
     * @param channel
     * @param sample
     * @return SampleType
     */
    SampleType processSample (const int channel, const float sample);
protected:
    
    //==========================================================================
    // Processing

    /**
     * @brief This method applies interpolation to the samples in the selected channel.
     *
     * @param channel: Channel from which the sample must be interpolated.
     * @return SampleType
     */
    virtual SampleType interpolateSample(const int channel) = 0;
    
    /**
     * @brief This method is used to update internal variables after the sample interpolation process.
     */
    virtual void updateInternalVariables() = 0;
    
    //==========================================================================
    // Buffer.
    juce::AudioBuffer <SampleType> buffer;
    int maxBufferSize;
    
    // Spec.
    double sampleRate;
    juce::uint32 numChannels;
    juce::uint32 maxBlocks;
    
    // Delay.
    float delaySamples = 0.f;
    float delayFrac = 0.f; // Depends on delay samples.
    int delayInt = 0; // Depends on delay samples.
    std::vector <int> writePointer;
    std::vector <int> readPointer;
    
    // Feedback.
    float feedback;
}; // class DelayLineBase
    

// Derived class from DelayLineBase implementing None interpolation for samples interpolation.
template <typename SampleType>
class DelayLineNone : public DelayLineBase<SampleType>
{
public:
    //==========================================================================
    // Default constructor.
    // Calling directly DelayLineBase, no constructor definition required.
    
    //==========================================================================
    // Destructor.
    
    /**
     *  DelayLineNone destructor
     */
    ~DelayLineNone() override {}

private:
    
    //==========================================================================
    // Processing

    /**
     * @brief This method applies None interpolation to the samples in the selected channel.
     *
     * @param channel: Channel from which the sample must be interpolated.
     * @return SampleType
     */
    
    SampleType interpolateSample(const int channel) override;

    /**
     * @brief This method is used to update internal variables after the sample None interpolation process.
     */
    void updateInternalVariables() override {}
    
}; // class DelayLineNone

} // namespace dsp
} // namespace cdrt

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>
#include "../utility/Interpolation.h"

namespace cdrt
{
namespace dsp
{
//template <typename SampleType, typename InterpolationType>
//class DelayLine
//{
//public:
//    //==========================================================================
//    // Default constructor.
//
//    /**
//     * @brief Construct a new DelayLine object.
//     * @param newMaxBufferSize: maximum acceptable size of the buffer, upper limit to max delay time.
//     */
//    DelayLine ();
//
//    //==========================================================================
//    // Allocation/Deallocation.
//
//    /**
//     * @brief Call this method before doing anything else to initialize the processor.
//     *
//     * @param spec: context informations for processor.
//     */
//    void prepare (const juce::dsp::ProcessSpec& spec);
//
//    /**
//     * @brief Initialize the members conserving a stae of the delay like the circular buffer.
//     *
//     */
//    void reset();
//
//    //==========================================================================
//    // Setters.
//
//    /**
//     * @brief Set the maxDelaySamples number.
//     *
//     * @param maxBufferSize: maximum acceptable size of the buffer, upper limit to max the delay time.
//     */
//    void setMaxDelaySamples (const int newMaxBufferSize);
//
//    /**
//     * @brief Set the delay time given a length expressed in samples.
//     *
//     * @param delaySamples: delay expressed in samples.
//     */
//    void setDelaySamples (const float newDelaySamples);
//
//    /**
//     * @brief Set the delay time given a length expressed in milliseconds.
//     *
//     * @param delayTime: delay expressed in milliseconds.
//     */
//    void setDelayTime (const float delayTime);
//
//    /**
//     * @brief Set the
//     *
//     * @param feedback
//     */
//    void setFeedback (const float newFeedback);
//
//    //==========================================================================
//    // Getters.
//
//    /**
//     * @brief Get the maximum delay in samples.
//     *
//     * @return int
//     */
//    int getMaximumDelaySamples() const noexcept;
//
//    /**
//     * @brief Get the value stored at a precise index and channel in the circular buffer without checks on the index value. This method exist for testing purposes but can be used as you prefer if you are creative enough :)
//     * Thi method will not interact with the values of read and write pointer, so you can read the same value many times without altering the buffer.
//     *
//     * @param channel: chanel where to get the sample from.
//     * @param index: value of the index to get the value from.
//     * @return SampleType
//     *
//     */
//    SampleType getSample (const int channel, const int index) const;
//
//    /**
//     * @brief Get the index where to read at given the setted delay in sapmles.
//     *
//     * @param channel: channel where to get the read index from.
//     * @return int
//     */
//    int getReadIndex(const int channel) const;
//
//    //==========================================================================
//    // Processing.
//
//    /**
//     * @brief put a sample into the selected channel.
//     *
//     * @param sample: value to store in circular buffer.
//     * @param channel: channel where to store the sample.
//     */
//    void putSample (const int channel, const SampleType sample);
//
//    /**
//     * @brief pop a sample from the given channel.
//     *
//     * @param channel: channel where to get the sample from.
//     * @param updatePointer: if you want to read again the same sample from the buffer set this to false.
//     * @return SampleType
//     */
//    SampleType popSample (const int channel, const bool updatePointer = true);
//
//    /**
//     * @brief TODO: Descritpion.
//     *
//     * @param channel
//     * @param sample
//     * @return SampleType
//     */
//    SampleType processSample (const int channel, const float sample);
//
//private:
//    //==========================================================================
//    // Processing
//
//    /**
//     * @brief This function is enabled only when the InterpolationType is Lagrange3rd and applies lagrange interpolation to the samples.
//     *
//     * @tparam T: Interpolation type.
//     * @param channel: Channel from which the sample must be interpolated.
//     * @return typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, SampleType>::type
//     */
//    template <typename T = InterpolationType>
//    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::None>::value, SampleType>::type
//    interpolateSample (int channel)
//    {
//        // Retriving index to read from.
//        const auto index = getReadIndex (channel);
//
//        // Retriving the sample from the index retrived in previous step.
//        return buffer.getSample(channel, index);
//    }
//
//    /**
//     * @brief This function is enabled only when the InterpolationType is Lagrange3rd and applies lagrange interpolation to the samples.
//     *
//     * @tparam T: Interpolation type.
//     * @param channel: Channel from which the sample must be interpolated.
//     * @return typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, SampleType>::type
//     */
//    template <typename T = InterpolationType>
//    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Linear>::value, SampleType>::type
//    interpolateSample (int channel)
//    {
//        // Retriving index to read from.
//        const auto index1 = getReadIndex (channel);
//        const auto index2 = (index1 + 1) % maxBufferSize;
//
//        // Retriving samples from indexes retrived in previous step.
//        const auto sample1 = buffer.getSample (channel, index1);
//        const auto sample2 = buffer.getSample (channel, index2);
//
//        auto res = cdrt::utility::interpolation::linear<SampleType> (sample1, sample2, delayFrac);
//
//        updateInternalVariables();
//
//        return res;
//    }
//
//    /**
//     * @brief This function is enabled only when the InterpolationType is Lagrange3rd and applies lagrange interpolation to the samples.
//     *
//     * @tparam T: Interpolation type.
//     * @param channel: Channel from which the sample must be interpolated.
//     * @return typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, SampleType>::type
//     */
//    template <typename T = InterpolationType>
//    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Lagrange3rd>::value, SampleType>::type
//    interpolateSample (int channel)
//    {
//        // Retriving index to read from.
//        auto index1 = getReadIndex (channel);
//        auto index2 = (index1 + 1) % maxBufferSize;
//        auto index3 = (index2 + 1) % maxBufferSize;
//        auto index4 = (index3 + 1) % maxBufferSize;
//
//        // Retriving samples from indexes retrived in previous step.
//        auto sample1 = buffer.getSample (channel, index1);
//        auto sample2 = buffer.getSample (channel, index2);
//        auto sample3 = buffer.getSample (channel, index3);
//        auto sample4 = buffer.getSample (channel, index4);
//
//        auto res = cdrt::utility::interpolation::lagrange3rd<SampleType> (sample1, sample2, sample3, sample4, delayFrac);
//
//        updateInternalVariables();
//
//        return res;
//    }
//
//    /**
//     * @brief This function is enabled only when the InterpolationType is Lagrange3rd and applies lagrange interpolation to the samples.
//     *
//     * @tparam T: Interpolation type.
//     * @param channel: Channel from which the sample must be interpolated.
//     * @return typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, SampleType>::type
//     */
//    template <typename T = InterpolationType>
//    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, SampleType>::type
//    interpolateSample (int channel)
//    {
//        // Retriving index to read from.
//        auto index1 = getReadIndex (channel);
//        auto index2 = (index1 + 1) % maxBufferSize;
//
//        // Retriving samples from indexes retrived in previous step.
//        auto sample1 = buffer.getSample (channel, index1);
//        auto sample2 = buffer.getSample (channel, index2);
//
//        // Calculating result and updating prev[channel] variable.
//        auto result = cdrt::utility::interpolation::thiran<SampleType> (sample1, sample2, delayFrac, alpha, prev[static_cast<size_t> (channel)]);
//
//        prev[static_cast<size_t> (channel)] = result;
//
//        updateInternalVariables();
//
//        return result;
//    }
//
//    //==========================================================================
//    /**
//     * @brief This method is enabled only when the InterpolationType is None and updates internal variables if any after the sample interpolation phase.“
//     *
//     * @tparam T: InterpolationType
//     * @return std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::None>::value, void>::type
//     */
//    template <typename T = InterpolationType>
//    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::None>::value, void>::type
//    updateInternalVariables()
//    {
//    }
//
//    /**
//     * @brief This method is enabled only when the InterpolationType is Linear and updates internal variables if any after the sample interpolation phase.“
//     *
//     * @tparam T: InterpolationType
//     * @return std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Linear>::value, void>::type
//     */
//    template <typename T = InterpolationType>
//    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Linear>::value, void>::type
//    updateInternalVariables()
//    {
//    }
//
//    /**
//     * @brief This method is enabled only when the InterpolationType is Linear and updates internal variables if any after the sample interpolation phase.“
//     *
//     * @tparam T: InterpolationType
//     * @return std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Lagrange3rd>::value, void>::type
//     */
//    template <typename T = InterpolationType>
//    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Lagrange3rd>::value, void>::type
//    updateInternalVariables()
//    {
//        if (delayInt >= 1)
//        {
//            delayFrac++;
//            delayInt--;
//        }
//    }
//
//    /**
//     * @brief This method is enabled only when the InterpolationType is Linear and updates internal variables if any after the sample interpolation phase.“
//     *
//     * @tparam T: InterpolationType
//     * @return std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, void>::type
//     */
//    template <typename T = InterpolationType>
//    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, void>::type
//    updateInternalVariables()
//    {
//        if (delayFrac < (SampleType) 0.618 && this->delayInt >= 1)
//        {
//            delayFrac++;
//            delayInt--;
//        }
//
//        alpha = (1 - this->delayFrac) / (1 + this->delayFrac);
//    }
//
//    //==========================================================================
//    // Buffer.
//    juce::AudioBuffer <SampleType> buffer;
//    int maxBufferSize;
//
//    // Spec.
//    double sampleRate;
//    juce::uint32 numChannels;
//    juce::uint32 maxBlocks;
//
//    // Delay.
//    float delaySamples = 0.f;
//    float delayFrac = 0.f; // Depends on delay samples.
//    int delayInt = 0; // Depends on delay samples.
//    std::vector <int> writePointer;
//    std::vector <int> readPointer;
//
//    // Feedback.
//    float feedback;
//
//    // Interpolation (Thiran only vars).
//    SampleType alpha;
//    std::vector <SampleType> prev;
//}; // class DelayLine


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
    DelayLineBase ();
    
    
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
private:
    
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
    // Destructor.
    
    /**
     *  DelayLineNone destructor
     */
    ~DelayLineNone() {}

private:
    
    //==========================================================================
    // Processing

    /**
     * @brief This method applies None interpolation to the samples in the selected channel.
     *
     * @param channel: Channel from which the sample must be interpolated.
     * @return SampleType
     */
    SampleType interpolateSample(const int channel)
    {
        // Retriving index to read from.
        const auto index = this->getReadIndex (channel);
        
        // Retriving the sample from the index retrived in previous step.
        return this->buffer.getSample(channel, index);
    }
    
    /**
     * @brief This method is used to update internal variables after the sample None interpolation process.
     */
    void updateInternalVariables() {}
    
}; // class DelayLineNone


// Derived class from DelayLineBase implementing Linear interpolation for samples interpolation.
template <typename SampleType>
class DelayLineLinear : public DelayLineBase<SampleType>
{
public:
    //==========================================================================
    // Destructor.
    
    /**
     *  DelayLineNone destructor
     */
    ~DelayLineLinear() {}

private:
    
    //==========================================================================
    // Processing

    /**
     * @brief This method applies linear interpolation to the samples in the selected channel.
     *
     * @param channel: Channel from which the sample must be interpolated.
     * @return SampleType
     */
    SampleType interpolateSample(const int channel)
    {
        // Retriving index to read from.
        const auto index1 = this->getReadIndex (channel);
        const auto index2 = (index1 + 1) % this->getMaximumDelaySamples();
        
        // Retriving samples from indexes retrived in previous step.
        const auto sample1 = this->buffer.getSample (channel, index1);
        const auto sample2 = this->buffer.getSample (channel, index2);
        
        auto res = cdrt::utility::interpolation::linear<SampleType> (sample1, sample2, this->delayFrac);
        
        updateInternalVariables();
        
        return res;
    }
    
    /**
     * @brief This method is used to update internal variables after the sample linear interpolation process.
     */
    void updateInternalVariables() {}
    
}; // class DelayLineNone

} // namespace dsp
} // namespace cdrt

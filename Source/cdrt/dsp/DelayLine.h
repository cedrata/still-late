#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>
#include "../utility/Interpolation.h"

namespace cdrt
{
namespace dsp
{
template <typename SampleType, typename InterpolationType>
class DelayLine
{
public:
    //==========================================================================
    // Default constructor.
    DelayLine (const int maxBufferSize);
    
    //==========================================================================
    // Allocation/Deallocation.
    void prepare (const juce::dsp::ProcessSpec& spec, bool singleChannel);
    void reset();
    
    //==========================================================================
    // Setters.
    void setDelaySamples (const SampleType &delaySamples);
    void setDelayTime (const SampleType &delayTime);
    void setFeedback (const float &feedback);
    
    //==========================================================================
    // Processing.
    void putSample (const SampleType sample);
    SampleType popSample();
    SampleType processSample (int channel, float inputSample);

private:
    //==========================================================================
    // Processing
    /**
     * @brief This function is enabled only when the InterpolationType is Lagrange3rd and applies lagrange interpolation to the samples.
     * 
     * @tparam T: Interpolation type.
     * @param channel: Channel from which the sample must be interpolated.
     * @param delayFrac: Fractionary part of the delay used to execute the interpolation.
     * @param readSample: Index in bufer wehre to start read for samples.
     * @return typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, SampleType>::type
     */
    template <typename T = InterpolationType>
    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::None>::value, SampleType>::type
    interpolateSample (int channel, size_t readSample)
    {
        return buffer.getSample(channel, static_cast<int>(readSample));
    }
    
    /**
     * @brief This function is enabled only when the InterpolationType is Lagrange3rd and applies lagrange interpolation to the samples.
     * 
     * @tparam T: Interpolation type.
     * @param channel: Channel from which the sample must be interpolated.
     * @param delayFrac: Fractionary part of the delay used to execute the interpolation.
     * @param readSample: Index in bufer wehre to start read for samples.
     * @return typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, SampleType>::type
     */
    template <typename T = InterpolationType>
    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Linear>::value, SampleType>::type
    interpolateSample (int channel, size_t readSample)
    {
        // Retriving index to read from.
        auto index1 = static_cast<int>(readSample);
        auto index2 = (index1 + 1) % buffer.size;
        
        // Retriving samples from indexes retrived in previous step.
        auto sample1 = buffer.getSample(channel, index1);
        auto sample2 = buffer.getSample(channel, index2);
        return cdrt::utility::interpolation::linear<SampleType>(sample1, sample2, this->delayFrac);
    }
    
    /**
     * @brief This function is enabled only when the InterpolationType is Lagrange3rd and applies lagrange interpolation to the samples.
     * 
     * @tparam T: Interpolation type.
     * @param channel: Channel from which the sample must be interpolated.
     * @param delayFrac: Fractionary part of the delay used to execute the interpolation.
     * @param readSample: Index in bufer wehre to start read for samples.
     * @return typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, SampleType>::type
     */
    template <typename T = InterpolationType>
    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Lagrange3rd>::value, SampleType>::type
    interpolateSample (int channel, size_t readSample)
    {
        // Retriving index to read from.
        auto index1 = static_cast<int>(readSample);
        auto index2 = (index1 + 1) % buffer.size;
        auto index3 = (index2 + 1) % buffer.size;
        auto index4 = (index3 + 1) % buffer.size;
        
        // Retriving samples from indexes retrived in previous step.
        auto sample1 = buffer.getSample(channel, index1);
        auto sample2 = buffer.getSample(channel, index2);
        auto sample3 = buffer.getSample(channel, index3);
        auto sample4 = buffer.getSample(channel, index4);
        
        return cdrt::utility::interpolation::lagrange3rd<SampleType>(sample1, sample2, sample3, sample4, this->delayFrac);
    }
    
    /**
     * @brief This function is enabled only when the InterpolationType is Lagrange3rd and applies lagrange interpolation to the samples.
     * 
     * @tparam T: Interpolation type.
     * @param channel: Channel from which the sample must be interpolated.
     * @param delayFrac: Fractionary part of the delay used to execute the interpolation.
     * @param readSample: Index in bufer wehre to start read for samples.
     * @return typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, SampleType>::type
     */
    template <typename T = InterpolationType>
    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, SampleType>::type
    interpolateSample (int channel, size_t readSample)
    {
        // Retriving index to read from.
        auto index1 = static_cast<int>(readSample);
        auto index2 = (index1 + 1) % buffer.size;
        
        // Retriving samples from indexes retrived in previous step.
        auto sample1 = buffer.getSample(channel, index1);
        auto sample2 = buffer.getSample(channel, index2);
        
        // Calculating result and updating prev[channel] variable.
        auto result = cdrt::utility::interpolation::thiran<float>(sample1, sample2, this->delayFrac, alpha, prev[channel]);
        
        prev[channel] = result;
        
        return result;
    }

    //==========================================================================
    /**
     * @brief This method is enabled only when the InterpolationType is None and updates internal variables if any after the sample interpolation phase.“
     * 
     * @tparam T: InterpolationType 
     * @return std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::None>::value, void>::type 
     */
    template <typename T = InterpolationType>
    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::None>::value, void>::type
    updateInternalVariables()
    {
    }

    /**
     * @brief This method is enabled only when the InterpolationType is Linear and updates internal variables if any after the sample interpolation phase.“
     * 
     * @tparam T: InterpolationType 
     * @return std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Linear>::value, void>::type 
     */
    template <typename T = InterpolationType>
    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Linear>::value, void>::type
    updateInternalVariables()
    {
    }

    /**
     * @brief This method is enabled only when the InterpolationType is Linear and updates internal variables if any after the sample interpolation phase.“
     * 
     * @tparam T: InterpolationType 
     * @return std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Lagrange3rd>::value, void>::type 
     */
    template <typename T = InterpolationType>
    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Lagrange3rd>::value, void>::type
    updateInternalVariables()
    {
        if (this->delayInt >= 1)
        {
            this->delayFrac++;
            this->delayInt--;
        }
    }

    /**
     * @brief This method is enabled only when the InterpolationType is Linear and updates internal variables if any after the sample interpolation phase.“
     * 
     * @tparam T: InterpolationType 
     * @return std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, void>::type 
     */
    template <typename T = InterpolationType>
    typename std::enable_if <std::is_same <T, cdrt::utility::interpolation::InterpolationTypes::Thiran>::value, void>::type
    updateInternalVariables()
    {
        if (this->delayFrac < (SampleType) 0.618 && this->delayInt >= 1)
        {
            this->delayFrac++;
            this->delayInt--;
        }

        this->alpha = (1 - this->delayFrac) / (1 + this->delayFrac);
    }

    //==========================================================================
    // Buffer.
    juce::AudioBuffer <SampleType> buffer;
    int maxBufferSize;
    
    // Spec.
    double sampleRate;
    juce::uint32 numChannels;
    juce::uint32 maxBlocks;
    
    // Delay.
    SampleType delaySamples = 0.f;
    SampleType delayFrac = 0.f; // Depends on delay samples.
    int delayInt = 0; // Depends on delay samples.
    std::vector <size_t> writePointer;
    
    // Feedback.
    float feedback;
    
    // Interpolation (Thiran only vars).
    SampleType alpha;
    std::vector <SampleType> prev;
};

} // namespace dsp
} // namespace cdrt

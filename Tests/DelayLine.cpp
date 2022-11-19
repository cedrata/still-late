#include <cdrt/dsp/DelayLine.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <juce_dsp/juce_dsp.h>

struct juce::dsp::ProcessSpec ps = {44100, 5, 1};
std::array<float, 20> inputSamples{-1.0f, -0.9f, -0.8f, -0.7f, -0.6f, -0.5f, -0.4f, -0.3f, -0.2f, -0.2f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};

// None interpolation.
TEST_CASE("Delay Line put/get sample w/ None interpolation.")
{
    auto dl = cdrt::dsp::DelayLine<float, cdrt::utility::interpolation::InterpolationTypes::None> (5);
    
    dl.prepare (ps);
    dl.reset();
    
    // Being the None interpolation only the int part of delay will be considered.
    dl.setDelaySamples (1.5f);
    dl.setFeedback (0.5f);
    
    float res;
    
    // Filling the DelayLine with enough samples to do then
    // pop a sample in the first position for the second time.
    // The sample pushed the first time is -1.0,
    // the sample pushed the second is -0.5.
    // Having a feedback of 0.5 the value in the first position
    // after writing again should be -1.0.
    for (size_t i = 0; i < 6; ++i)
    {
        res = dl.processSample (0, inputSamples[i]);
    }
    
    res = dl.popSample (0);
    
    REQUIRE(res == -1.0f);
}

// Linear interpolation.
TEST_CASE("Delay Line put/get sample w/ Linear interpolation.")
{
    auto dl = cdrt::dsp::DelayLine<float, cdrt::utility::interpolation::InterpolationTypes::Linear> (5);
    
    dl.prepare (ps);
    dl.reset();
    
    dl.setDelaySamples (1.5f);
    dl.setFeedback (0.5f);
    
    float res;
    
    // Filling the DelayLine with enough samples to do then
    // pop a sample in the first position for the second time.
    // The sample pushed the first time is -1.0,
    // the sample pushed the second is -0.5.
    // Having a feedback of 0.5 the value in the first position
    // after writing again should be -1.0.
    for (size_t i = 0; i < 6; ++i)
    {
        res = dl.processSample (0, inputSamples[i]);
    }
    
    // The buffer at the index 4 will should be filled with the value 0.85.
    // This because it will to the interpolation between 0
    // (current value at index 4, last one)
    // and -1 value at index one.
    
    // The insert at the 6th iteration will be between the value at 1st and 2nd indexes:
    // -1, and -0.9 to multiply to the feedback and add to the value
    // of the inputSamples[5] = -0.5.
    res = dl.popSample (0);
    float expected = cdrt::utility::interpolation::linear<float> (inputSamples[0], inputSamples[1], 0.5f) * 0.5f + inputSamples[5];
    
    REQUIRE(res == expected);
}

// Lagrange3rd interpolation.
// Thiran interpolation.
// Those tests are missing at the moment. They are not required as they
// take inspiration from the linear interpolation type.
// Make a test for them would be too difficult and the effort is not worth to me.
// In future i will maybe add them if required.
// ...

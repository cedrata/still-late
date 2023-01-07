#include <cdrt/dsp/DelayLine.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <juce_dsp/juce_dsp.h>

struct juce::dsp::ProcessSpec ps = {44100, 5, 1};
std::array<float, 20> inputSamples{-1.0f, -0.9f, -0.8f, -0.7f, -0.6f, -0.5f, -0.4f, -0.3f, -0.2f, -0.2f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};

// None interpolation.
TEST_CASE("Delay Line put/get sample w/ None interpolation.")
{
    cdrt::dsp::DelayLine<float, cdrt::utility::interpolation::InterpolationTypes::None> dl;
    
    dl.prepare (ps);
    dl.setMaxDelaySamples(5);
    dl.reset();
    
    // Being the None interpolation only the int part of delay will be considered.
    dl.setDelaySamples (1.4f);
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
    
    res = dl.getSample (0, 0);
    float expected = inputSamples[5] + dl.getSample(0, 4) * 0.5f;
    
    REQUIRE(res == expected);
}

// Linear
// Lagrange3rd interpolation.
// Thiran interpolation.
// Those tests are missing at the moment. They are not required as they
// follow the same steps as the None interpolation the the interpolation functions already have been tested.
// Make a test for them would be too difficult and the effort is not worth to me.
// In future i will maybe add them if required.
// ...

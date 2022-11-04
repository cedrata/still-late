#include <catch2/catch_test_macros.hpp>

// Module to test.
#include <cdrt/utility/Conversion.h>

// msToSamples w/ dougble sampletype.
TEST_CASE("Convert 100ms to samples at 44100Hz: expected result -> 4410.0")
{
    REQUIRE(cdrt::utility::conversion::msToSamples<double>(100.0f, 44100.0f) == 4410.0f);
}

// msToSamples w/ float sampletype.
// At the moment not implemented, double is good enough as test.
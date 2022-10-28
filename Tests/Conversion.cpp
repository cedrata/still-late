#include <catch2/catch_test_macros.hpp>

// Module to test.
#include <cdrt/utility/Conversion.h>

// Test 
TEST_CASE("Convert 100ms to samples at 44100Hz: expected result -> 4410.0")
{
    REQUIRE(cdrt::utility::conversion::msToSamples(100.0, 44100) == 4410.0);
}

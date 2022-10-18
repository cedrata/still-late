#include <catch2/catch_test_macros.hpp>

// Module to test.
#include <cdrt/utility/Interpolation.h>

// Linear interpolation with float sampletype.
TEST_CASE("Linear interpolation <float>: with delayFrac=0 expected result -> sample1")
{
    using namespace cdrt::utility::interpolation;
    auto sample1 = 0.5f;
    auto sample2 = 0.25f;
    auto delayFrac = 0.f;
    
    REQUIRE(linear<float>(sample1, sample2, delayFrac) == sample1);
}

TEST_CASE("Linear interpolation <float>: with delayFrac=1 expected result -> sample2")
{
    using namespace cdrt::utility::interpolation;
    auto sample1 = 0.5f;
    auto sample2 = 0.25f;
    auto delayFrac = 1.f;
    
    REQUIRE(linear<float>(sample1, sample2, delayFrac) == sample2);
}

// This test is redoundant, can be uncommented to check this is working.
// Testing dummy/limit cases to se if the functions behave as they should.
// TEST_CASE("Linear interpolation <float>: with delayFrac=0.5 expected result -> 0.375f")
// {
//     using namespace cdrt::utility::interpolation;
//
//     auto sample1 = 0.5f;
//     auto sample2 = 0.25f;
//     auto delayFrac = 0.5f;
//
//     REQUIRE(linear<float>(sample1, sample2, delayFrac) == 0.375f);
// }

// Linear interpolation w/ double sampletype.
// At the moment not implemented, float is good enough as test.

// Lagrange3rd interpolation with float sampletype.
TEST_CASE("Lagrange3rd interpolation <float>: with delayFrac=0 expected result -> sample1")
{
    using namespace cdrt::utility::interpolation;
    auto sample1 = 0.75f;
    auto sample2 = 0.5f;
    auto sample3 = 0.25f;
    auto sample4 = 0.0f;
    auto delayFrac = 0.f;

    REQUIRE(lagrange3rd<float>(sample1, sample2, sample3, sample4, delayFrac) == sample1);
}

TEST_CASE("Lagrange3rd interpolation <float>: with delayFrac=1 expected result -> sample2")
{
    using namespace cdrt::utility::interpolation;
    auto sample1 = 0.75f;
    auto sample2 = 0.51f;
    auto sample3 = 0.24f;
    auto sample4 = 0.01f;
    auto delayFrac = 1.f;

    REQUIRE(lagrange3rd<float>(sample1, sample2, sample3, sample4, delayFrac) == sample2);
}

// This test is redoundant, can be uncommented to check this is working.
// Testing dummy/limit cases to se if the functions behave as they should.
// TEST_CASE("Lagrange3rd interpolation <float>: with delayFrac=0.5 expected result -> 0.625f")
// {
//     using namespace cdrt::utility::interpolation;
//     auto sample1 = 0.75f;
//     auto sample2 = 0.5f;
//     auto sample3 = 0.25f;
//     auto sample4 = 0.0f;
//     auto delayFrac = 0.5f;
//
//     REQUIRE(lagrange3rd<float>(sample1, sample2, sample3, sample4, delayFrac) == 0.625f);
// }

// Lagrange3rd interpolation w/ double sampletype.
// At the moment not implemented, float is good enough as test.

// Thiran interpolation with float sampletype.
TEST_CASE("Thiran interpolation <float>: with delayFrac=0 expected result -> sample1")
{
    using namespace cdrt::utility::interpolation;
    auto sample1 = 0.75f;
    auto sample2 = 0.5f;
    auto delayFrac = 0.0f;
    auto alpha = (1.0f - delayFrac) / (1.0f + delayFrac);
    auto prev = 0.6f; // not important the value of it, given the value of delayFrac it will not be used.
    

    REQUIRE(thiran<float>(sample1, sample2, delayFrac, alpha, prev) == sample1);
}

TEST_CASE("Thiran interpolation <float>: with delayFrac=1 expected result -> sample2")
{
    using namespace cdrt::utility::interpolation;
    auto sample1 = 0.75f;
    auto sample2 = 0.5f;
    auto delayFrac = 1.0f;
    auto alpha = (1.0f - delayFrac) / (1.0f + delayFrac);
    auto prev = 0.6f; // not important the value of it, given the value of delayFrac it will not be used.
    

    REQUIRE(thiran<float>(sample1, sample2, delayFrac, alpha, prev) == sample2);
}

// Thiran interpolation w/ double sampletype.
// At the moment not implemented, float is good enough as test.

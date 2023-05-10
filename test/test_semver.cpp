#include "semver.hpp"

#include "catch2/catch_test_macros.hpp"


TEST_CASE("SemVer from zeros", "[semver]") {
	CHECK(cpm::SemVer(0, 0, 0).string() == "0.0.0");
}

TEST_CASE("SemVer from numbers", "[semver]") {
	CHECK(cpm::SemVer(1, 2, 3).string() == "1.2.3");
}

TEST_CASE("SemVer from string", "[semver]") {
	CHECK(cpm::SemVer("1.2.3").string() == "1.2.3");
}

TEST_CASE("SemVer from empty string", "[semver]") {
	CHECK_THROWS(cpm::SemVer(""));
}

TEST_CASE("is SemVer specified", "[semver]") {
	CHECK_FALSE(cpm::SemVer().is_specified());
}

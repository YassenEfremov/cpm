#include "util.hpp"

#include "catch2/catch_test_macros.hpp"

#include <vector>


TEST_CASE("normal split by delimiter", "[split_string]") {

	CHECK(cpm::util::split_string("hello/world/!", "/") == std::vector<std::string>{"hello", "world", "!"});
}

TEST_CASE("empty string", "[split_string]") {
	CHECK(cpm::util::split_string("", "/") == std::vector<std::string>{""});
}

TEST_CASE("delimeter missing in string", "[split_string]") {
	CHECK(cpm::util::split_string("hello", "/") == std::vector<std::string>{"hello"});
}

TEST_CASE("string surrounded by delimiters", "[split_string]") {
	CHECK(cpm::util::split_string("/hello/", "/") == std::vector<std::string>{"hello"});
}

TEST_CASE("repeated delimiter", "[split_string]") {
	CHECK(cpm::util::split_string("hello", "l") == std::vector<std::string>{"he", "o"});
}

TEST_CASE("long delimiter", "[split_string]") {
	CHECK(cpm::util::split_string("helloasdworld", "asd") == std::vector<std::string>{"hello", "world"});
}

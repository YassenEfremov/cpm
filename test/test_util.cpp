#include "util.hpp"

#include "catch2/catch_test_macros.hpp"

#include <string>
#include <vector>


TEST_CASE("split_string", "[split_string]") {

	SECTION("normal split by delimiter") {
		CHECK(cpm::util::split_string("hello/world/!", "/") == std::vector<std::string>{"hello", "world", "!"});
	}

	SECTION("empty string") {
		CHECK(cpm::util::split_string("", "/") == std::vector<std::string>{""});
	}

	SECTION("delimeter missing in string") {
		CHECK(cpm::util::split_string("hello", "/") == std::vector<std::string>{"hello"});
	}

	SECTION("string surrounded by delimiters") {
		CHECK(cpm::util::split_string("/hello/", "/") == std::vector<std::string>{"hello"});
	}

	SECTION("repeated delimiter") {
		CHECK(cpm::util::split_string("hello", "l") == std::vector<std::string>{"he", "o"});
	}

	SECTION("long delimiter") {
		CHECK(cpm::util::split_string("helloasdworld", "asd") == std::vector<std::string>{"hello", "world"});
	}
}

TEST_CASE("base64_decode", "[base64_decode]") {

	SECTION("decode base64 encoded string") {
		CHECK(cpm::util::base64_decode("SGVsbG8gV29ybGQh") == "Hello World!");
	}

	SECTION("decode empty string") {
		CHECK(cpm::util::base64_decode("") == "");
	}
}

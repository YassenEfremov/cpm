#include "semver.hpp"

#include "spdlog/fmt/ostr.h"

#include "util.hpp"

#include <stdexcept>
#include <string>


namespace cpm {

SemVer::SemVer() : major(0), minor(0), patch(0) {}

SemVer::SemVer(int major, int minor, int patch)
	: major(major), minor(minor), patch(patch) {}

SemVer::SemVer(const std::string &version_str) {
	auto tokens = util::split_string(version_str, ".");
	if (tokens.size() != 3) {
		throw std::invalid_argument(version_str + ": invalid version string!");
	}
	this->major = std::stoi(tokens[0]);
	this->minor = std::stoi(tokens[1]);
	this->patch = std::stoi(tokens[2]);
}

int SemVer::get_major() const { return this->major; }
int SemVer::get_minor() const { return this->minor; }
int SemVer::get_patch() const { return this->patch; }

std::string SemVer::string() const {
	return fmt::format("{}.{}.{}", this->major, this->minor, this->patch);
}

bool SemVer::is_specified() {
	return this->string() != "0.0.0";
}

bool operator<(const SemVer& l, const SemVer& r) {
	if (l.major < r.major) {
		return true;
	}
	if (l.minor < r.minor) {
		return true;
	}
	if (l.patch < r.patch) {
		return true;
	}
	return false;
}

bool operator> (const SemVer& lhs, const SemVer& rhs) {
	return rhs < lhs;
}

bool operator<=(const SemVer& lhs, const SemVer& rhs) {
	return !(lhs > rhs);
}

bool operator>=(const SemVer& lhs, const SemVer& rhs) {
	return !(lhs < rhs);
}

bool operator==(const SemVer& l, const SemVer& r) {
	return l.major == r.major && l.minor == r.minor && l.patch == r.patch;
}

bool operator!=(const SemVer &lhs, const SemVer &rhs) {
	return !(lhs == rhs);
}

} // namespace cpm

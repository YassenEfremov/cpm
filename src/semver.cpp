#include "semver.hpp"

#include "util.hpp"

#include <stdexcept>
#include <string>


namespace cpm {

	SemVer::SemVer() : major(0), minor(0), patch(0), suffix("") {}

	SemVer::SemVer(int major, int minor, int patch, const std::string &suffix)
		: major(major), minor(minor), patch(patch), suffix(suffix) {}

	SemVer::SemVer(const std::string &version_str) {
		auto tokens = util::split_string(version_str, ".");
		if (tokens.size() < 3) {
			throw std::invalid_argument(version_str + ": invalid version string!");
		}
		this->major = std::stoi(tokens[0]);
		this->minor = std::stoi(tokens[1]);
		this->patch = std::stoi(tokens[2]);
		this->suffix = "";
	}

	std::string SemVer::to_string() const {
		std::string str = std::to_string(this->major) + "." +
			   			  std::to_string(this->minor) + "." +
			   			  std::to_string(this->patch);
		
		if (!this->suffix.empty()) {
			str += "-" + this->suffix; 
		}
		return str;
	}

	bool SemVer::is_specified() {
		return this->to_string() != "0.0.0";
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
}

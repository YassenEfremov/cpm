#include "package.hpp"

#include <functional>
#include <string>

#include <cstddef>


namespace cpm {

	bool Package::operator==(const Package &other) const {
		return this->name == other.name;
	}

	std::size_t Package::PackageHash::operator()(const Package &package) const noexcept {
		std::size_t h1 = std::hash<std::string>{}(package.name);
		return h1 << 1;
	}
}

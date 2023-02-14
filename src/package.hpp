#ifndef PACKAGE_H
#define PACKAGE_H

// #include "semver.hpp"

#include <string>
#include <unordered_set>

#include <cstddef>


namespace cpm {

	/**
	 * @brief A struct representing a cpm package
	 */
	struct Package {

		struct PackageHash {
			std::size_t operator()(const Package &package) const noexcept;
		};

		std::string name;
		// SemVer version;
		// std::unordered_set<Package, Package::PackageHash> dependencies;

		friend bool operator==(const Package &lhs, const Package &rhs);
	};
}


#endif	// PACKAGE_H

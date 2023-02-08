#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>

#include <cstddef>


namespace cpm {

	/**
	 * @brief A struct representing a cpm package
	 */
	struct Package {

		std::string name;
		// Version version;

		bool operator==(const Package &other) const;

		struct PackageHash {
			std::size_t operator()(const Package &package) const noexcept;
		};
	};
}


#endif	// PACKAGE_H

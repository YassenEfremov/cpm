#ifndef PACKAGE_H
#define PACKAGE_H

#include "semver.hpp"

#include <memory>
#include <string>
#include <unordered_set>

#include <cstddef>


namespace cpm {

/**
 * @brief A class representing a cpm package
 */
class Package {

	public:

	struct Hash {
		std::size_t operator()(const Package &package) const noexcept;
	};

	/**
	 * @brief Constructor for package
	 * 
	 * @param name the name of the package
	 */
	Package(const std::string &name);

	/**
	 * @brief Constructor for package
	 * 
	 * @param name the name of the package
	 * @param version the package version
	 */
	Package(const std::string &name, const SemVer &version);

	/**
	 * @brief Getter for name
	 * 
	 * @return The name of the package
	 */
	std::string get_name() const;

	/**
	 * @brief Getter for version
	 * 
	 * @return The package version
	 */
	SemVer get_version() const;

	/**
	 * @brief Getter for location
	 * 
	 * @return The location of the package
	 */
	std::string get_location() const;

	/**
	 * @brief Getter for dependencies
	 * 
	 * @return The package's dependencies
	 */
	std::shared_ptr<std::unordered_set<Package, Package::Hash>>
	get_dependencies() const;

	/**
	 * @brief Get the package as a string
	 * 
	 * @return The package as a string
	 */
	std::string string() const;

	/**
	 * @brief Initialize the package location and resolve it's version
	 */
	void init();

	friend bool operator==(const Package &lhs, const Package &rhs);


	private:

	std::string name;
	SemVer version;
	std::string location;
	std::string url;
	std::string description;
	std::string author;
	std::string license;
	std::shared_ptr<std::unordered_set<Package, Package::Hash>> dependencies;
};

} // namespace cpm


#endif	// PACKAGE_H

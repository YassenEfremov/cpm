#ifndef LOCKFILE_H
#define LOCKFILE_H

#include "package.hpp"
#include "repository.hpp"

#include "nlohmann/json.hpp"

#include <filesystem>
#include <string>
#include <unordered_set>

namespace fs = std::filesystem;


namespace cpm {

/**
 * @brief A class representing the package lockfile
 */
class Lockfile : public Repository<Package, Package::Hash> {

	public:

	/**
	 * @brief Constructor for package lockfile
	 * 
	 * @param filename the name of the package lockfile
	 */
	Lockfile(const fs::path &filename);

	/**
	 * @brief Add the specified dependency to the package lockfile
	 * 
	 * @param package the package to add
	 * 
	 * @return The number of records modified in the package lockfile
	 */
	int add(const Package &package) override;

	/**
	 * @brief Add the specified transitive dependency to an already existing
	 * 		  package
	 * 
	 * @param existing_package the package to add the dependency to
	 * @param dep the new dependency
	 * 
	 * @return The number of records modified in the package lockfile
	 */
	int add_dep(const Package &existing_package, const Package &dep);

	/**
	 * @brief Remove the specified dependency from the package lockfile
	 * 
	 * @param package the package to remove
	 * 
	 * @return The number of records modified in the package lockfile
	 */
	int remove(const Package &package) override;

	/**
	 * @brief Remove the specified transitive dependency from an already
	 * 		  existing package
	 * 
	 * @param existing_package the package to remove the dependency from
	 * @param dep the dependency
	 * 
	 * @return The number of records modified in the package lockfile
	 */
	int remove_dep(const Package &existing_package, const Package &dep);

	/**
	 * @brief List all of the installed dependencies in the package lockfile
	 * 
	 * @return A map of all the installed packages in the package lockfile
	 */
	std::unordered_set<Package, Package::Hash> list() const override;

	/**
	 * @brief Check if the specified dependency is in the package lockfile
	 * 
	 * @param package the package
	 * 
	 * @return true if the package is found, false otherwise
	 */
	bool contains(const Package &package) const override;

	/**
	 * @brief Check if the specified transitive dependency is in the package
	 * 		  lockfile
	 * 
	 * @param package the package
	 * 
	 * @return true if the package is found, false otherwise
	 */
	bool contains_dep(const Package &package);


	private:

	nlohmann::ordered_json lockfile_json;

	/**
	 * @brief Save the current lockfile_json to a file with the current filename
	 */
	void save();
};

} // namespace cpm


#endif	// LOCKFILE_H

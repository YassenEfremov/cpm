#ifndef CPM_PACK_H
#define CPM_PACK_H

#include "package.hpp"
#include "repository.hpp"

#include "nlohmann/json.hpp"

#include <filesystem>
#include <string>
#include <unordered_set>

namespace fs = std::filesystem;


namespace cpm {

	/**
	 * @brief A class representing the package config
	 */
	class PackageConfig : public Repository<Package, Package::Hash> {

		public:

		/**
		 * @brief Constructor for package config
		 * 
		 * @param filename the name of the package config file
		 */
		PackageConfig(const fs::path &filename);

		/**
		 * @brief Add the specified package/s to the package config file
		 * 
		 * @param package the package to add
		 * 
		 * @return The number of lines modified in the package config file
		 */
		int add(const Package &package) override;

		/**
		 * @brief Remove the specified package/s from the package config file
		 * 
		 * @param package the package to remove
		 * 
		 * @return The number of lines modified in the package config file
		 */
		int remove(const Package &package) override;

		/**
		 * @brief List all of the installed packages in the package config file
		 * 
		 * @return A map of all the installed packages in the package config
		 * 		   file
		 */
		std::unordered_set<Package, Package::Hash> list() const override;

		/**
		 * @brief Check if the specified package is in the package config file
		 * 
		 * @param package the package
		 * 
		 * @return true if the package is found, false otherwise
		 */
		bool contains(const Package &package) const override;

		/**
		 * @brief Create a new package with the default field values in the
		 * 		  current package config file
		 * 
		 * @return The created package
		 */
		Package create_default();


		private:

		nlohmann::ordered_json config_json;

		/**
		 * @brief Save the current config_json to a file with the current
		 * 		  filename
		 */
		void save();
	};
}


#endif	// CPM_PACK_H

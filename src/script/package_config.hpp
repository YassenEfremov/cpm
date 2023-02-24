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
	 * @brief A class representing the cpm_pack.json
	 */
	class PackageConfig : public Repository<Package, Package::Hash> {

		public:

		/**
		 * @brief Constructor for cpm_pack.json
		 * 
		 * @param filename the name of the cpm_pack.json file
		 */
		PackageConfig(const fs::path &filename);

		/**
		 * @brief Add the specified package/s to the cpm_pack.json file
		 * 
		 * @param package the package to add
		 * 
		 * @return The number of lines modified in the cpm_pack.json file
		 */
		int add(const cpm::Package &package) override;

		/**
		 * @brief Remove the specified package/s from the cpm_pack.json file
		 * 
		 * @param package the package to remove
		 * 
		 * @return The number of lines modified in the cpm_pack.json file
		 */
		int remove(const cpm::Package &package) override;

		/**
		 * @brief List all of the installed packages in the cpm_pack.json file
		 * 
		 * @return A map of all the installed packages in the cpm_pack.json
		 * 		   file
		 */
		std::unordered_set<Package, Package::Hash> list() override;

		/**
		 * @brief Check if the specified package is in the cpm_pack.json file
		 * 
		 * @param package the package
		 * 
		 * @return true if the package is found, false otherwise
		 */
		bool contains(const Package &package) override;

		/**
		 * @brief Create a new package with the default field values in the
		 * 		  current cpm_pack.json file
		 * 
		 * @return The created package
		 */
		Package create();


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

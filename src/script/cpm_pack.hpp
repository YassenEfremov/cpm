#ifndef CPM_PACK_H
#define CPM_PACK_H

#include "../package.hpp"
#include "../repository.hpp"

#include "nlohmann/json.hpp"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


namespace cpm {

	class CPMPack : public Repository<Package> {

		private:

		nlohmann::ordered_json cpm_pack_json;

		/**
		 * @brief Save the current cpm_pack_json to a file with the current
		 * 		  filename
		 */
		void save();


		public:

		CPMPack(const fs::path &filename);

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
		 * @return A list of all the installed packages in the cpm_pack.json
		 * 		   file
		 */
		std::vector<Package> list() override;

		/**
		 * @brief Create a new package with the default field values in the
		 * 		  current cpm_pack.json file
		 * 
		 * @return The created package
		 */
		Package create();
	};
}


#endif	// CPM_PACK_H


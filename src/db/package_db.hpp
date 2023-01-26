#ifndef PACKAGE_DB_H
#define PACKAGE_DB_H

#include "../package.hpp"
#include "../repository.hpp"

#include "sqlite3.h"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


namespace cpm {

	/**
	 * @brief A class representing the local package DB
	 */
	class PackageDB : public Repository<Package> {

		private:

		sqlite3 *package_db;


		public:

		PackageDB(const fs::path &filename);
		~PackageDB() override;

		/**
		 * @brief Add the specified package/s to the package DB
		 * 
		 * @param package the package to add
		 * 
		 * @return The number of rows modified in the DB
		 */
		int add(const Package &package) override;

		/**
		 * @brief Remove the specified package/s from the package DB
		 * 
		 * @param package the package to remove
		 * 
		 * @return The number of rows modified in the DB
		 */
		int remove(const Package &package) override;

		/**
		 * @brief List all of the installed packages in the package DB
		 * 
		 * @return A list of all the installed packages in the DB
		 */
		std::vector<Package> list() override;
	};
}


#endif	// PACKAGE_DB_H


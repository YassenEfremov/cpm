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
	 * @brief A class representing the local package database
	 */
	class PackageDB : public Repository<Package> {

		public:

		/**
		 * @brief Constructor for package database
		 * 
		 * @param filename the name of the database file
		 */
		PackageDB(const fs::path &filename);
		~PackageDB() override;

		/**
		 * @brief Add the specified package/s to the package database
		 * 
		 * @param package the package to add
		 * 
		 * @return The number of rows modified in the database
		 */
		int add(const Package &package) override;

		/**
		 * @brief Remove the specified package/s from the package database
		 * 
		 * @param package the package to remove
		 * 
		 * @return The number of rows modified in the database
		 */
		int remove(const Package &package) override;

		/**
		 * @brief List all of the installed packages in the package database
		 * 
		 * @return A list of all the installed packages in the database
		 */
		std::vector<Package> list() override;


		private:

		sqlite3 *package_db;
	};
}


#endif	// PACKAGE_DB_H


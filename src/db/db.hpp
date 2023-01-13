#ifndef DB_H
#define DB_H

#include "sqlite3.h"

#include "../package.hpp"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


namespace cpm {

	class PackageDB {

		private:

		sqlite3 *db;


		public:

		PackageDB(const fs::path &name);
		~PackageDB();

		/**
		 * @brief Add the given package/s to the package DB
		 * 
		 * @param package the package name
		 * 
		 * @return The number of rows modified in the DB
		 */
		int add(const cpm::Package &package);

		/**
		 * @brief Remove the given package/s from the package DB
		 * 
		 * @param package the package name
		 * 
		 * @return The number of rows modified in the DB
		 */
		int remove(const cpm::Package &package);

		/**
		 * @brief List all of the installed packages in the package DB
		 * 
		 * @return A list of all the installed packages in the DB
		 */
		std::vector<Package> list();
	};
}


#endif	// DB_H


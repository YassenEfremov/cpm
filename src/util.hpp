#ifndef UTIL_H
#define UTIL_H

#include <filesystem>

namespace fs = std::filesystem;


namespace cpm::util {

	/**
	 * @brief The directory under which packages are installed
	 */
	extern const fs::path packages_dir;

	/**
	 * @brief The SQLite DB file which stores the globally installed packages
	 */
	extern const fs::path package_db;

	/**
	 * @brief The current package config file
	 */
	extern const fs::path package_config;

	/**
	 * @brief The directory in which cpm operates
	 */
	extern const fs::path global_dir;
}


#endif	// UTIL_H

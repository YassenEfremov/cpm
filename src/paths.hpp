#ifndef PATHS_H
#define PATHS_H

#include <filesystem>
#include <string>	

namespace fs = std::filesystem;


namespace cpm::paths {

	/**
	 * @brief The directory under which packages are installed
	 */
	extern const fs::path packages_dir;

	/**
	 * @brief The SQLite database file which stores the globally installed
	 * 		  packages
	 */
	extern const fs::path package_db;

	/**
	 * @brief The current package config file
	 */
	extern const fs::path package_config;

	/**
	 * @brief The current package lockfile
	 */
	extern const fs::path lockfile;

	/**
	 * @brief The directory in which cpm operates
	 */
	extern const fs::path global_dir;

	/**
	 * @brief The API url which cpm should use to download packages
	 */
	extern const fs::path api_url;

	/**
	 * @brief The file which contains the locations where cpm should search for
	 * 		  packages
	 */
	extern const fs::path package_locations;
}


#endif	// PATHS_H

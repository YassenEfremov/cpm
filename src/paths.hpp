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
 * @brief The SQLite database file which stores the globally installed packages
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
 * @brief The file which contains the locations where cpm should search for
 * 		  packages
 */
extern const fs::path package_locations;

/**
 * @brief The directory in which logs are stored
 */
extern const fs::path logs_dir;

/**
 * @brief The base name of the log files
 */
extern const fs::path log_base_name;

/**
 * @brief The API URL which cpm should use to download packages
 */
extern const std::string api_url;

/**
 * @brief GitHub API identifier for zip archive
 */
extern const std::string gh_zip;

/**
 * @brief GitHub API identifier for repo tags
 */
extern const std::string gh_tags;

/**
 * @brief GitHub API identifier for repo contents
 */
extern const std::string gh_content;

} // namespace cpm::paths


#endif	// PATHS_H

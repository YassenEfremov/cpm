#ifndef UTIL_H
#define UTIL_H

#include <filesystem>

namespace fs = std::filesystem;


namespace cpm::util {

	const fs::path packages_dir = "lib";
	const fs::path packages_db = "packages.db3";
	const fs::path package_config = "cpm_pack.json";
}


#endif	// UTIL_H

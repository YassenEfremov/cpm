#ifndef UTIL_H
#define UTIL_H

#include <filesystem>
#include <string>

namespace fs = std::filesystem;


namespace cpm::util {

	const fs::path packages_dir = "lib";
	const fs::path packages_db = "packages.db3";
}


#endif	// UTIL_H

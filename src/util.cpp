#include "util.hpp"

#include <cstdlib>

#include <filesystem>

namespace fs = std::filesystem;


namespace cpm::util {

	const fs::path packages_dir = "lib";

	const fs::path package_db = "packages.db3";

	const fs::path package_config = "cpm_pack.json";

#ifdef _WIN32
	const fs::path global_dir = std::getenv("LOCALAPPDATA") / fs::path("cpm");
#elif defined(__unix__)
	const fs::path global_dir = std::getenv("HOME") / fs::path(".local/share/cpm");
#endif
}

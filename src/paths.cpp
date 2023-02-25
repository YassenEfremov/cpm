#include "paths.hpp"

#include <filesystem>
#include <string>

#include <cstdlib>

namespace fs = std::filesystem;


namespace cpm::paths {

	const fs::path packages_dir = "lib";

	const fs::path package_db = "packages.db3";

	const fs::path package_config = "cpm_pack.json";

#ifdef _WIN32
	const fs::path global_dir = std::getenv("LOCALAPPDATA") / fs::path("cpm");
#elif defined(__unix__)
	const fs::path global_dir = std::getenv("HOME") / fs::path(".local/share/cpm");
#endif

	const fs::path api_url = "https://api.github.com/repos";

	const fs::path package_locations = "package_locations.json";
}

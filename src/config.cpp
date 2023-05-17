#include "config.hpp"

#include "paths.hpp"

#include <filesystem>

namespace fs = std::filesystem;


namespace cpm {

void Config::init() {
	if (!fs::exists(paths::global_dir / paths::package_locations)) {

#ifdef _WIN32
		fs::copy_file("C:\\Program Files (x86)\\cpm" / paths::package_locations, paths::global_dir / paths::package_locations);
#elif defined(__unix__)
		fs::copy_file("/etc/cpm" / paths::package_locations, paths::global_dir / paths::package_locations);
#endif
	}
}

} // namespace cpm

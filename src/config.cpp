#include "config.hpp"

#include "paths.hpp"

#include <filesystem>

namespace fs = std::filesystem;


namespace cpm {

void Config::init() {
	if (!fs::exists(paths::global_dir / paths::package_locations)) {
		fs::copy_file(paths::install_prefix / "share" / "cpm" / paths::package_locations, paths::global_dir / paths::package_locations);
	}
}

} // namespace cpm

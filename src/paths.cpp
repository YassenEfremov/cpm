#include "paths.hpp"
#include "install_prefix.hpp"

#include <filesystem>
#include <string>

#include <cstdlib>

namespace fs = std::filesystem;


namespace cpm::paths {

const fs::path install_prefix = INSTALL_PREFIX;

const fs::path packages_dir = "lib";

const fs::path package_db = "packages.db3";

const fs::path package_config = "cpm_pack.json";

const fs::path lockfile = "cpm_lock.json";

#ifdef _WIN32
const fs::path global_dir = std::getenv("APPDATA") / fs::path("cpm");
#elif defined(__unix__)
const fs::path global_dir = std::getenv("HOME") / fs::path(".local/share/cpm");
#endif

const fs::path package_locations = "package_locations.json";

const fs::path logs_dir = "logs";

const fs::path log_base_name = "cpm.log";

const std::string api_url = "https://api.github.com/repos";

const std::string gh_zip = "zipball";

const std::string gh_tags = "tags";

const std::string gh_content = "contents";

} // namespace cpm::paths

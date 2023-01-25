#include "list.hpp"

#include "command.hpp"
#include "../db/package_db.hpp"
#include "../package.hpp"
#include "../script/cpm_pack.hpp"
#include "../util.hpp"

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;


namespace cpm {

    ListCommand::ListCommand(const std::string &name) : Command(name) {}

    void ListCommand::run() {

        std::vector<cpm::Package> installed_packages;

        fs::path cwd;
        if (this->is_used("--global")) {
            cwd = util::global_dir;

        } else {
            cwd = fs::current_path();
        }

        if (this->is_used("--global")) {
            if (!fs::exists(cwd / util::package_db)) {
                spdlog::info("No packages installed!\n");
                return;
            }
            PackageDB db(cwd / util::package_db);
            installed_packages = db.list();
            if (installed_packages.empty()) {
                spdlog::info("No packages installed!\n");
                return;
            }

        } else {
            if (!fs::exists(cwd / util::package_config)) {
                spdlog::info("No packages installed!\n");
                return;
            }
            CPMPack cpm_pack(cwd / util::package_config);
            installed_packages = cpm_pack.list();
            if (installed_packages.empty()) {
                spdlog::info("No packages installed!\n");
                return;
            }
        }

        spdlog::info("Packages installed in {}:\n", (cwd / util::packages_dir / "").string());
        for (auto p : installed_packages) {
            spdlog::info("   {}\n", p.get_name());
        }
	}
}

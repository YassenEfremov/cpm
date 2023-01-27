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

        fs::path cwd;
        if (this->is_used("--global")) {
            cwd = util::global_dir;

        } else {
            cwd = fs::current_path();
        }

        std::vector<cpm::Package> installed_packages;

        if (this->is_used("--global")) {
            if (!fs::exists(cwd / util::package_db)) {
                // this check prevents the package database from being created
                // when there are obviously no packages in it
                spdlog::info("No packages installed!\n");
                return;
            }
            PackageDB db(cwd / util::package_db);
            installed_packages = db.list();

        } else {
            CPMPack cpm_pack(cwd / util::package_config);
            installed_packages = cpm_pack.list();
        }

        if (installed_packages.empty()) {
            spdlog::info("No packages installed!\n");
            return;
        }

        spdlog::info("Packages installed in {}:\n", (cwd / util::packages_dir / "").string());
        for (const auto &p : installed_packages) {
            spdlog::info("   {}\n", p.get_name());
        }
	}
}

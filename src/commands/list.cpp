#include "commands/list.hpp"

#include "commands/command.hpp"
#include "db/package_db.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/cpm_pack.hpp"
#include "semver.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <string>
#include <unordered_set>

namespace fs = std::filesystem;


namespace cpm {

    ListCommand::ListCommand(const std::string &name) : Command(name) {}

    void ListCommand::run() {

        if (!fs::exists(this->context.cwd / this->context.repo->get_filename())) {
            spdlog::info("No packages installed!\n");
            return;
        }

        auto installed_packages = this->context.repo->list();

        if (installed_packages.empty()) {
            spdlog::info("No packages installed!\n");
            return;
        }

        spdlog::info("Packages in {}:\n", (this->context.cwd / paths::packages_dir / "").string());
        for (const auto &package : installed_packages) {
            if (!fs::exists(this->context.cwd / paths::packages_dir / package.get_name())) {
                spdlog::info("   {}@{} (not installed)\n", package.get_name(), package.get_version().to_string());
                
            } else {
                spdlog::info("   {}@{}\n", package.get_name(), package.get_version().to_string());
            }
        }

        for (const auto &dir_entry : fs::directory_iterator(this->context.cwd / paths::packages_dir / "")) {

            Package package(dir_entry.path().filename().string());

            if (installed_packages.find(package) == installed_packages.end()) {
                spdlog::info("   {}@{} (unspecified)\n", package.get_name(), package.get_version().to_string());
            }
        }
	}
}

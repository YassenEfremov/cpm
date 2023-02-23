#include "commands/list.hpp"

#include "commands/command.hpp"
#include "db/package_db.hpp"
#include "logger/logger.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/cpm_pack.hpp"
#include "semver.hpp"

#include <filesystem>
#include <string>
#include <unordered_set>

namespace fs = std::filesystem;


namespace cpm {

    ListCommand::ListCommand(const std::string &name) : Command(name) {}

    void ListCommand::run() {

        CPM_LOG_INFO(">>>>> Starting list command >>>>>");

        if (!fs::exists(this->context.cwd / this->context.repo->get_filename())) {
            CPM_LOG_ERR("Repository doesn't exitst in the current context!");
            CPM_INFO("No packages installed!\n");
            return;
        }

        auto installed_packages = this->context.repo->list();

        if (installed_packages.empty()) {
            CPM_LOG_ERR("Set of installed packages is empty!");
            CPM_INFO("No packages installed!\n");
            return;
        }

        CPM_INFO("Packages in {}:\n", (this->context.cwd / paths::packages_dir / "").string());
        for (const auto &package : installed_packages) {
            if (!fs::exists(this->context.cwd / paths::packages_dir / package.get_name())) {
                CPM_INFO("   {}@{} (not installed)\n", package.get_name(), package.get_version().to_string());
                
            } else {
                CPM_INFO("   {}@{}\n", package.get_name(), package.get_version().to_string());
            }
        }

        if (fs::exists(this->context.cwd / paths::packages_dir / "")) {
            for (const auto &dir_entry : fs::directory_iterator(this->context.cwd / paths::packages_dir / "")) {

                Package package(dir_entry.path().filename().string());

                if (installed_packages.find(package) == installed_packages.end()) {
                    CPM_INFO("   {}@{} (unspecified)\n", package.get_name(), package.get_version().to_string());
                }
            }
        }

        CPM_LOG_INFO(">>>>> Finished list. >>>>>");
	}
}

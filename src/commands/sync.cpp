#include "commands/sync.hpp"

#include "cli/colors.hpp"
#include "commands/command.hpp"
#include "commands/install.hpp"
#include "commands/remove.hpp"
#include "logger/logger.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/package_config.hpp"
#include "semver.hpp"
#include "util.hpp"

#include "spdlog/fmt/ostr.h"

#include <exception>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_set>

#include <cstdint>

namespace fs = std::filesystem;


namespace cpm {

SyncCommand::SyncCommand(const std::string &name) : Command(name), InstallCommand(name), RemoveCommand(name) {}

void SyncCommand::run() {

    CPM_LOG_INFO("===== Starting sync command =====");

    CPM_LOG_INFO("Obtaining new packages from {} ...",paths::package_config.string());
    auto packages = this->context.repo->list();
    CPM_LOG_INFO("New packages in {}: {}", paths::package_config.string(),
        [&]() {
            std::string packages_str = "[";
            for (const auto &p : packages) {
                packages_str += p.get_name() + ", ";
            }
            return packages_str + "]";
        }()
    );

    CPM_INFO("Synchronizing packages with {} ...\n", paths::package_config.string());

    CPM_LOG_INFO("Initializing packages ...",paths::package_config.string());
    decltype(packages) packages_to_install;
    for (auto package : packages) {
        CPM_LOG_INFO("Checking package {} ...", package.string());
        CPM_INFO("Checking package {} ...", package.string());
        try {
            package.init();
        } catch(const std::exception &e) {
            CPM_INFO(" failed!\n");
            throw std::invalid_argument(e.what());
        }
        CPM_LOG_INFO(
            "version {} for package {} is valid",
            package.get_version().string(), package.get_name()
        );
        CPM_INFO(" found.\n");
        packages_to_install.insert(package);
    }

    CPM_LOG_INFO("Installing new packages ...",paths::package_config.string());
    int new_packages = 0;
    for (const auto &package : packages_to_install) {
        try {
            new_packages += this->install_package(
                package, this->context.cwd / paths::packages_dir / ""
            );
        
        } catch(const std::exception &e) {
            CPM_LOG_ERR(e.what());
            CPM_ERR(e.what());
        }
    }

    CPM_LOG_INFO("Removing unspecified packages ...", paths::package_config.string());
    int unspecified_packages = 0;
    for (const auto &dir_entry : fs::directory_iterator(this->context.cwd / paths::packages_dir / "")) {

        if (fs::is_symlink(dir_entry.path())) continue;
        auto tokens = util::split_string(dir_entry.path().filename().string(), "@");
        if (tokens.size() < 2) continue;
        Package package(tokens[0], SemVer(tokens[1]));

        if (!this->context.repo->contains(package) &&
            !this->context.lockfile->contains_dep(package)) {
            CPM_LOG_INFO("Found unspecified package {}, removing", package.string());
            unspecified_packages += this->remove_package(package);
        }
    }

    CPM_INFO(
        "Syncronization complete.\n"
        "Installed {} new, removed {} unspecified\n",
        new_packages, unspecified_packages
    );

    CPM_LOG_INFO("===== Finished sync. =====");

    this->final_message(packages);
}

int SyncCommand::install_package(const Package &package,
                                    const fs::path &output_dir) {

    if (this->check_if_installed(package)) {
        CPM_INFO("{}: Already installed, skipping ...\n", package.string());
        return 0;
    }
    
    CPM_INFO(
        "{}: Installing into {} ...\n", package.string(),
        (output_dir / package.get_name() / "").string()
    );

    this->install_all(package, package.get_location(), output_dir);

    return 1;
}

bool SyncCommand::check_if_installed(const Package &package) {
    return fs::exists(this->context.cwd / paths::packages_dir / package.get_name() / "");
}

int SyncCommand::remove_package(const Package &package) {

    CPM_INFO("{}: Not specified, removing ...\n", package.string());
    
    std::uintmax_t n = this->delete_all(package);
    CPM_INFO(RED_FG(" - ") "Removed {} entries\n", n);

    return 1;
}

} // namespace cpm

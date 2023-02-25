#include "commands/sync.hpp"

#include "commands/command.hpp"
#include "commands/install.hpp"
#include "commands/remove.hpp"
#include "logger/logger.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/package_config.hpp"
#include "semver.hpp"
#include "util.hpp"

#include <exception>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>

#include <cstdint>

namespace fs = std::filesystem;


namespace cpm {

    SyncCommand::SyncCommand(const std::string &name) : Command(name), InstallCommand(name), RemoveCommand(name) {}

    void SyncCommand::run() {

        CPM_LOG_INFO("===== Starting sync command =====");

        CPM_LOG_INFO("Obtaining new packages from {} ...",paths::package_config.string());
        PackageConfig cpm_pack(fs::current_path() / paths::package_config);
        auto packages = cpm_pack.list();
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

        CPM_LOG_INFO("Initializing new packages ...",paths::package_config.string());
        for (auto package : packages) {
            CPM_LOG_INFO("Checking package {} ...", package.string());
            CPM_INFO("Checking package {} ...", package.string());
            try {
                package.find_location();
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
        }

        CPM_LOG_INFO("Installing new packages ...",paths::package_config.string());
        int new_packages = 0;
        for (auto package : packages) {
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

            Package package(dir_entry.path().filename().string());

            if (packages.find(package) == packages.end()) {
                CPM_LOG_INFO("Found unspecified package {}, removing", package.get_name());
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
            CPM_INFO(package.get_name() + ": Already installed, skipping ...");
            return 0;
        }
        
        CPM_INFO(
            "{}: Installing into {} ...\n", package.string(),
            (output_dir / package.get_name() / "").string()
        );

        this->install_deps(package, output_dir);

        return 1;
    }

	bool SyncCommand::check_if_installed(const Package &package) {
        return fs::exists(this->context.cwd / paths::packages_dir / package.get_name() / "");
    }

	int SyncCommand::remove_package(const Package &package) {

        CPM_INFO("{}: Not specified, removing ...\n", package.get_name());
        
        std::uintmax_t n = this->delete_package(package);
        CPM_INFO("    Removed {} entries\n", n);

        return 1;
    }
}

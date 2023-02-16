#include "commands/sync.hpp"

#include "commands/command.hpp"
#include "commands/install.hpp"
#include "commands/remove.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/cpm_pack.hpp"
#include "semver.hpp"
#include "util.hpp"

#include "spdlog/spdlog.h"

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

        CPMPack cpm_pack(fs::current_path() / paths::package_config);
        auto packages = cpm_pack.list();

        spdlog::info("Synchronizing packages with {} ...\n", paths::package_config.string());

        for (auto package : packages) {
            spdlog::info("Checking package {}@{} ...", package.get_name(), package.get_version().to_string());
            try {
                package.init();
            } catch(const std::exception &e) {
                spdlog::info(" not found!\n");
                throw std::invalid_argument(e.what());
            }
            spdlog::info(" done.\n");
        }

        int new_packages = 0;
        for (auto package : packages) {
            try {
                new_packages += this->install_package(
                    package, this->context.cwd / paths::packages_dir / ""
                );
            
            } catch(const std::exception &e) {
                spdlog::get("stderr_logger")->error(e.what());
            }
        }

        int unspecified_packages = 0;
        for (const auto &dir_entry : fs::directory_iterator(this->context.cwd / paths::packages_dir / "")) {

            Package package(dir_entry.path().filename().string());

            if (packages.find(package) == packages.end()) {
                unspecified_packages += this->remove_package(package);
            }
        }

        spdlog::info(
            "Syncronization complete.\n"
            "Installed {} new, removed {} unspecified\n",
            new_packages, unspecified_packages
        );
	}

    int SyncCommand::install_package(const Package &package,
                                     const fs::path &output_dir) {

        try {
            this->check_if_installed(package);
        } catch(const std::exception &e) {
            spdlog::info("{}\n", e.what());
            return 0;
        }
        
        spdlog::info(
            "{}@{}: Installing into {} ...\n", package.get_name(), package.get_version().to_string(),
            (output_dir / package.get_name() / "").string()
        );

        this->install_deps(package, output_dir);

        return 1;
    }

	void SyncCommand::check_if_installed(const Package &package) {
        if (fs::exists(this->context.cwd / paths::packages_dir / package.get_name() / "")) {
            throw std::invalid_argument(package.get_name() + ": Already installed, skipping ...");
        }
    }

	int SyncCommand::remove_package(const Package &package) {

        spdlog::info("{}: Not specified, removing ...\n", package.get_name());
        
        std::uintmax_t n = this->delete_package(package);
        spdlog::info("    Removed {} entries\n", n);

        return 1;
    }
}

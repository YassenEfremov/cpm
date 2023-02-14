#include "commands/sync.hpp"

#include "commands/command.hpp"
#include "commands/install.hpp"
#include "commands/remove.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/cpm_pack.hpp"
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

        int new_packages = 0;
        for (const auto &package : packages) {
            try {
                new_packages += this->install_package(
                    package, this->context.cwd / paths::packages_dir / ""
                );
            
            } catch(const std::exception &e) {
                spdlog::get("stderr_logger")->error(e.what());
            }
        }

        int unneeded_packages = 0;
        for (const auto &dir_entry : fs::directory_iterator(this->context.cwd / paths::packages_dir / "")) {

            Package package{dir_entry.path().filename().string()};

            if (packages.find(package) == packages.end()) {
                unneeded_packages += this->remove_package(package);
            }
        }

        spdlog::info(
            "Syncronization complete.\n"
            "Installed {} new, removed {} unneeded\n",
            new_packages, unneeded_packages
        );
	}

    int SyncCommand::install_package(const Package &package,
                                     const fs::path &output_dir) {

        this->check_if_installed(package);
        spdlog::info(
            "{}: Installing into {} ...\n", package.name,
            (output_dir / package.name / "").string()
        );

        this->install_deps(package, output_dir);

        return 1;
    }


	void SyncCommand::check_if_installed(const Package &package) {
        if (fs::exists(this->context.cwd / paths::packages_dir / package.name / "")) {
            throw std::invalid_argument(package.name + ": Already installed, skipping ...");
        }
    }

	int SyncCommand::remove_package(const Package &package) {

        spdlog::info("{}: Not specified, removing ...\n", package.name);
        
        std::uintmax_t n = this->delete_package(package);
        spdlog::info("    Removed {} entries\n", n);

        return 1;
    }
}

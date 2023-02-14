#include "commands/remove.hpp"

#include "commands/command.hpp"
#include "db/package_db.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/cpm_pack.hpp"

#include "spdlog/spdlog.h"

#include <exception>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include <cstdint>

namespace fs = std::filesystem;


namespace cpm {

    RemoveCommand::RemoveCommand(const std::string &name) : Command(name) {}

    void RemoveCommand::run() {

        auto package_names = this->get<std::vector<std::string>>("packages");
        // auto packages = std::vector<Package>(package_names.begin(), package_names.end());
        std::unordered_set<Package, Package::PackageHash> packages;
        for (const auto &name : package_names) {
            packages.insert(Package{name});
        }

        int records_modified = 0;
        for (const auto &package : packages) {
            try {
                records_modified += this->remove_package(package);

            } catch(const std::exception &e) {
                spdlog::get("stderr_logger")->error(e.what());
                continue;
            }
        }

        spdlog::info(
            "{}: modified {} record/s\n",
            this->context.repo->get_filename().filename().string(), records_modified
        );
    }

    int RemoveCommand::remove_package(const Package &package) {

        this->check_if_not_installed(package);
        spdlog::info(
            "Removing package from {} ...\n",
            (this->context.cwd / paths::packages_dir / package.name / "").string()
        );

        std::uintmax_t n = this->delete_package(package);
        spdlog::info("Removed {} entries\n", n);

        return this->unregister_package(package);
    }

    void RemoveCommand::check_if_not_installed(const Package &package) {
        bool specified = this->context.repo->contains(package);
        bool installed = fs::exists(this->context.cwd / paths::packages_dir / package.name / "");

        if (!specified && !installed) {
            throw std::invalid_argument(package.name + ": package not installed!");
        }
    }

	std::uintmax_t RemoveCommand::delete_package(const Package &package) {
        return fs::remove_all(this->context.cwd / paths::packages_dir / package.name / "");
    }

	int RemoveCommand::unregister_package(const Package &package) {
        return this->context.repo->remove(package);
    }
}

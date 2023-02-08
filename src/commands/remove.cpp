#include "commands/remove.hpp"

#include "commands/command.hpp"
#include "db/package_db.hpp"
#include "package.hpp"
#include "script/cpm_pack.hpp"
#include "paths.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdint>

namespace fs = std::filesystem;


namespace cpm {

    RemoveCommand::RemoveCommand(const std::string &name) : Command(name) {}

    void RemoveCommand::run() {

        auto package_names = this->get<std::vector<std::string>>("packages");
        // auto packages = std::vector<Package>(package_names.begin(), package_names.end());
        std::vector<Package> packages;
        for (const auto &name : package_names) {
            packages.push_back(Package{name});
        }

        for (const auto &package : packages) {

            // Check if the target directory exists
            if (!fs::exists(this->context.cwd / paths::packages_dir / package.name / "")) {            // TODO: check the local package database instead
                throw std::invalid_argument("Package not installed!");
            }

            spdlog::info(
                "Removing package from {} ...\n",
                (this->context.cwd / paths::packages_dir / package.name / "").string()
            );

            std::uintmax_t n = fs::remove_all(this->context.cwd / paths::packages_dir / package.name / "");
            spdlog::info("Removed {} entries\n", n);

            int records_modified = this->context.repo->remove(package);
            spdlog::info(
                "{}: modified {} record/s\n",
                this->context.repo->get_filename().filename().string(), records_modified
            );
        }
    }
}

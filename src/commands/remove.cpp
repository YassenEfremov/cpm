#include "commands/remove.hpp"

#include "commands/command.hpp"
#include "db/package_db.hpp"
#include "logger/logger.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/package_config.hpp"
#include "semver.hpp"
#include "util.hpp"

#include "spdlog/fmt/ostr.h"

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

        CPM_LOG_INFO("===== Starting remove command =====");

        auto packages_str = this->get<std::vector<std::string>>("packages");
        CPM_LOG_INFO("args: {}", [&](){
            std::string args = "[";
            for (const auto &arg : packages_str) {
                args += arg + ", ";
            }
            return args + "]";
        }());
        std::unordered_set<Package, Package::Hash> packages;
        for (const auto &package_str : packages_str) {
            packages.insert(Package(package_str));
        }

        int records_modified = 0;
        for (const auto &package : packages) {
            try {
                records_modified += this->remove_package(package);

            } catch(const std::exception &e) {
                CPM_LOG_ERR(e.what());
                CPM_ERR(e.what());
            }
        }

        CPM_INFO(
            "{}: modified {} record/s\n",
            this->context.repo->get_filename().filename().string(), records_modified
        );

        CPM_LOG_INFO("===== Finished remove. =====");
    }

    int RemoveCommand::remove_package(const Package &package) {

        CPM_LOG_INFO("Checking if package {} is not installed ...", package.get_name());
        if (this->check_if_not_installed(package)) {
            throw std::invalid_argument(fmt::format(
                "{}: package not installed!", package.get_name()
            ));
        }

        CPM_INFO(
            "Removing package from {} ...\n",
            (this->context.cwd / paths::packages_dir / package.get_name() / "").string()
        );

        CPM_LOG_INFO(
            "Removing package from {} ...\n",
            (this->context.cwd / paths::packages_dir / package.get_name() / "").string()
        );
        std::uintmax_t n = this->delete_all(package);
        CPM_LOG_INFO("Removed {} entries", n);
        CPM_INFO(" \x1b[31m-\x1b[37m Removed {} entries\n", n);

        CPM_LOG_INFO("Removing package from {} ...", paths::package_config.string());
        return this->unregister_package(package);
    }

    bool RemoveCommand::check_if_not_installed(const Package &package) {
        bool specified = this->context.repo->contains(package);
        bool downloaded = fs::exists(this->context.cwd / paths::packages_dir / package.get_name() / "");

        return !specified || !downloaded;
    }

	std::uintmax_t RemoveCommand::delete_all(const Package &package) {
        std::uintmax_t total_entries = 0;
        if (fs::exists(this->context.cwd / paths::packages_dir /
                       package.get_name() / paths::packages_dir / "")) {
            for (const auto &dep : fs::directory_iterator(this->context.cwd / paths::packages_dir /
                                                        package.get_name() / paths::packages_dir / "")) {
                total_entries += this->delete_all(Package(dep.path().filename().string()));
            }
        }
        total_entries += fs::remove_all(this->context.cwd / paths::packages_dir / package.get_name() / "");
        return total_entries;
    }

	int RemoveCommand::unregister_package(const Package &package) {
        return this->context.repo->remove(package);
    }
}

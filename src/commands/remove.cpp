#include "commands/remove.hpp"

#include "commands/command.hpp"
#include "cli/colors.hpp"
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

    auto package_names = this->get<std::vector<std::string>>("packages");
    CPM_LOG_INFO("args: {}", [&](){
        std::string args = "[";
        for (const auto &arg : package_names) {
            args += arg + ", ";
        }
        return args + "]";
    }());
    std::unordered_set<Package, Package::Hash> packages;
    for (const auto &package_name : package_names) {
        auto tokens = util::split_string(package_name, "@");
        if (tokens.size() == 2) {
            CPM_LOG_INFO("Checking package {}@{} ...", tokens[0], tokens[1]);
            CPM_INFO("Checking package {}@{} ...", tokens[0], tokens[1]);
            Package new_package(tokens[0], SemVer(tokens[1]));
            if (!fs::exists(this->context.cwd / paths::packages_dir / new_package.string())) {
                CPM_INFO(" failed!\n");
                throw std::invalid_argument(fmt::format(
                    "{}: package not installed!", new_package.string()
                ));
            }
            CPM_LOG_INFO(
                "version {} for package {} is valid",
                new_package.get_version().string(), new_package.get_name()
            );
            CPM_INFO(" found.\n");
            packages.insert(new_package);

        } else if (tokens.size() == 1) {
            CPM_LOG_INFO("Resolving version for package {} ...", tokens[0]);
            CPM_INFO("Resolving version for package {} ...", tokens[0]);
            fs::path package_path;
            try {
                package_path = fs::read_symlink(this->context.cwd / paths::packages_dir / package_name);
            } catch(const std::exception &e) {
                CPM_INFO(" failed!\n");
                throw std::invalid_argument(fmt::format(
                    "{}: package not installed!", package_name
                ));
            }
            auto tokens = util::split_string(package_path.filename().string(), "@");
            Package new_package(tokens[0], SemVer(tokens[1]));
            CPM_LOG_INFO(
                "found version {} (latest) for package {}",
                new_package.get_version().string(), new_package.get_name()
            );
            CPM_INFO(" found latest: " YELLOW_FG("{}") "\n", new_package.get_version().string());
            packages.insert(new_package);

        } else {
            throw std::invalid_argument(fmt::format(
                "{}: invalid package format!", package_name
            ));
        }
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

    CPM_LOG_INFO("Checking if package {} is not installed ...", package.string());
    if (this->check_if_not_installed(package)) {
        throw std::invalid_argument(fmt::format(
            "{}: package not installed!", package.string()
        ));
    }

    CPM_INFO(
        "Removing package from {} ...\n",
        (this->context.cwd / paths::packages_dir / package.get_name() / "").string()
    );

    CPM_LOG_INFO(
        "Removing package from {} ...",
        (this->context.cwd / paths::packages_dir / package.get_name() / "").string()
    );
    std::uintmax_t n = this->delete_all(package);
    CPM_LOG_INFO("Removed {} entries", n);
    CPM_INFO(RED_FG(" - ") "Removed {} entries\n", n);

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
    this->context.lockfile->remove(package);
    if (fs::exists(this->context.cwd / paths::packages_dir / package.get_name() / paths::packages_dir / "")) {
        for (const auto &dep_path : fs::directory_iterator(this->context.cwd / paths::packages_dir /
                                                           package.get_name() / paths::packages_dir / "")) {
            fs::path dep_path_real = fs::read_symlink(dep_path);
            auto tokens = util::split_string(dep_path_real.filename().string(), "@");
            if (tokens.size() < 2) continue;
            Package dep(tokens[0], SemVer(tokens[1]));
            CPM_LOG_INFO("Checking transitive dependency {} ...", dep.string());
            if (this->context.repo->contains(dep) || this->context.lockfile->contains_dep(dep)) {
                CPM_LOG_INFO(
                    "Transitive dependency {} is required by another package! Skipping ...",
                    dep.string()
                );
                continue;
            }
            total_entries += this->delete_all(dep);
            CPM_LOG_INFO("Removed transitive dependency {}", dep.string());
        }
    }
    if (this->context.lockfile->contains_dep(package)) {
        CPM_LOG_INFO(
            "Package {} is required by another package! Skipping ...",
            package.string()
        );

    } else {
        if (fs::exists(this->context.cwd / paths::packages_dir / package.get_name()) &&
            fs::is_symlink(this->context.cwd / paths::packages_dir / package.get_name()) &&
            fs::read_symlink(this->context.cwd / paths::packages_dir / package.get_name()) ==
            this->context.cwd / paths::packages_dir / package.string()) {
            fs::remove(this->context.cwd / paths::packages_dir / package.get_name());
        }
        total_entries += fs::remove_all(this->context.cwd / paths::packages_dir / package.string() / "");
    }
    return total_entries;
}

int RemoveCommand::unregister_package(const Package &package) {
    return this->context.repo->remove(package);
}

} // namespace cpm

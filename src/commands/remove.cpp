#include "commands/remove.hpp"

#include "commands/command.hpp"
#include "db/package_db.hpp"
#include "logger/logger.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/cpm_pack.hpp"
#include "semver.hpp"
#include "util.hpp"

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

        CPM_LOG_INFO(
            ">>>>> Starting remove command with args: {} >>>>>",
            [&]() {
                std::string packages_str = "[";
                for (const auto &p : this->get<std::vector<std::string>>("packages")) {
                    packages_str += p + ", ";
                }
                return packages_str + "]";
            }()
        );

        auto packages_str = this->get<std::vector<std::string>>("packages");
        std::unordered_set<Package, Package::Hash> packages;
        for (const auto &package_str : packages_str) {
            packages.insert(Package(package_str));
        }

        int records_modified = 0;
        for (const auto &package : packages) {
            try {
                records_modified += this->remove_package(package);

            } catch(const std::exception &e) {
                CPM_ERR(e.what());
            }
        }

        CPM_INFO(
            "{}: modified {} record/s\n",
            this->context.repo->get_filename().filename().string(), records_modified
        );

        CPM_LOG_INFO("<<<<< Finished remove. <<<<<");
    }

    int RemoveCommand::remove_package(const Package &package) {

        this->check_if_not_installed(package);
        CPM_INFO(
            "Removing package from {} ...\n",
            (this->context.cwd / paths::packages_dir / package.get_name() / "").string()
        );

        std::uintmax_t n = this->delete_package(package);
        CPM_INFO(" \x1b[31m-\x1b[37m Removed {} entries\n", n);

        return this->unregister_package(package);
    }

    void RemoveCommand::check_if_not_installed(const Package &package) {
        bool specified = this->context.repo->contains(package);
        bool installed = fs::exists(this->context.cwd / paths::packages_dir / package.get_name() / "");

        if (!specified && !installed) {
            throw std::invalid_argument(package.get_name() + ": package not installed!");
        }
    }

	std::uintmax_t RemoveCommand::delete_package(const Package &package) {
        return fs::remove_all(this->context.cwd / paths::packages_dir / package.get_name() / "");
    }

	int RemoveCommand::unregister_package(const Package &package) {
        return this->context.repo->remove(package);
    }
}

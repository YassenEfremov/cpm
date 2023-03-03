#include "commands/list.hpp"

#include "commands/command.hpp"
#include "db/package_db.hpp"
#include "logger/logger.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/package_config.hpp"
#include "semver.hpp"

#include <filesystem>
#include <string>
#include <unordered_set>

namespace fs = std::filesystem;


namespace cpm {

ListCommand::ListCommand(const std::string &name) : Command(name) {}

void ListCommand::run() {

    CPM_LOG_INFO("===== Starting list command =====");

    CPM_LOG_INFO("Checking if a {} file exists ...", paths::package_config.string());
    if (!fs::exists(this->context.cwd / this->context.repo->get_filename())) {
        CPM_LOG_INFO("Repository doesn't exist in the current context!");
        CPM_INFO("No packages installed!\n");
        return;
    }

    auto installed_packages = this->context.repo->list();

    CPM_LOG_INFO("Checking if there are any packages installed ...");
    if (installed_packages.empty() && (!fs::exists(this->context.cwd / paths::packages_dir / "") ||
                                       fs::is_empty(this->context.cwd / paths::packages_dir / ""))) {
        CPM_LOG_INFO("Set of installed packages is empty!");
        CPM_INFO("No packages installed!\n");
        return;
    }

    CPM_LOG_INFO("Listing packages specified in {} ...",
                    (this->context.cwd / paths::package_config).string());
    CPM_INFO("Packages in {}:\n",
                (this->context.cwd / paths::packages_dir / "").string());
    int installed = 0;
    int not_installed = 0;
    for (const auto &package : installed_packages) {
        if (!fs::exists(this->context.cwd / paths::packages_dir / package.get_name())) {
            not_installed++;
            CPM_INFO("  {} (not installed)\n", package.string());

        } else {
            installed++;
            if (this->is_used("--all")) {
                this->print_deps(package, this->context.cwd / paths::packages_dir / package.get_name() / "");
            } else {
                CPM_INFO("  {}\n", package.string());
            }
        }
    }

    CPM_LOG_INFO("Listing packages NOT specified in {} ...",
                    (this->context.cwd / paths::package_config).string());
    int unspecified = 0;
    if (fs::exists(this->context.cwd / paths::packages_dir / "")) {
        for (const auto &dir_entry : fs::directory_iterator(this->context.cwd / paths::packages_dir / "")) {
            Package package(dir_entry.path().filename().string());
            if (!this->context.repo->contains(package) &&
                !this->context.lockfile->contains_dep(package)) {
                unspecified++;
                CPM_INFO("  {} (unspecified)\n", package.get_name());
            }
        }
    }

    CPM_INFO("\nTotal: {} installed, {} not installed, {} unspecified\n",
                installed, not_installed, unspecified);

    CPM_LOG_INFO("===== Finished list command. =====");
}

void ListCommand::print_deps(const Package &package, const fs::path dir) {
    static int depth = 1;
    for (int i = 0; i < depth; i++) {
        CPM_INFO("  ");
    }
    // if (depth > 1) {
    //     CPM_INFO("└─ ");
    // }
    if (depth > 1 && this->context.lockfile->contains(package)) {
        CPM_INFO("{} (symlinked)\n", package.string());
        
    } else {
        CPM_INFO("{}\n", package.string());
    }

    PackageConfig package_config(dir / paths::package_config);
    auto deps = package_config.list();
    depth++;
    for (const auto &dep : deps) {
        print_deps(dep, dir / paths::packages_dir / dep.get_name() / "");
    }
    depth--;
}

} // namespace cpm

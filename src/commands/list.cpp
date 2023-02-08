#include "commands/list.hpp"

#include "commands/command.hpp"
#include "db/package_db.hpp"
#include "package.hpp"
#include "script/cpm_pack.hpp"
#include "paths.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;


namespace cpm {

    ListCommand::ListCommand(const std::string &name) : Command(name) {}

    void ListCommand::run() {

        if (!fs::exists(this->context.cwd / this->context.repo->get_filename())) {
            spdlog::info("No packages installed!\n");
            return;
        }

        auto installed_packages = this->context.repo->list();

        if (installed_packages.empty()) {
            spdlog::info("No packages installed!\n");
            return;
        }

        spdlog::info("Packages in {}:\n", (this->context.cwd / paths::packages_dir / "").string());
        for (const auto &p : installed_packages) {
            if (!fs::exists(this->context.cwd / paths::packages_dir / p.name)) {
                spdlog::info("   {} (not installed)\n", p.name);
                
            } else {
                spdlog::info("   {}\n", p.name);
            }
        }
	}
}

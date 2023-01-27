#include "remove.hpp"

#include "command.hpp"
#include "../db/package_db.hpp"
#include "../package.hpp"
#include "../script/cpm_pack.hpp"
#include "../util.hpp"

#include "nlohmann/json.hpp"
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
        auto packages = std::vector<Package>(package_names.begin(), package_names.end());

        fs::path cwd;
        if (this->is_used("--global")) {
            cwd = util::global_dir;
        } else {
            cwd = fs::current_path();
        }

        for (const auto &package : packages) {

            // Check if the target directory exists
            if (!fs::exists(cwd / util::packages_dir / package.get_name() / "")) {            // TODO: check the local package database instead
                throw std::invalid_argument("Package not installed!");
            }

            spdlog::info(
                "Removing package from {} ...\n",
                (cwd / util::packages_dir / package.get_name() / "").string()
            );

            std::uintmax_t n = fs::remove_all(cwd / util::packages_dir / package.get_name() / "");
            spdlog::info("Removed {} entries\n", n);

            if (this->is_used("--global")) {
                PackageDB db(cwd / util::package_db);
                int rows_modified = db.remove(package);
                spdlog::info("Package DB: modified {} rows\n", rows_modified);

            } else {
                CPMPack cpm_pack(cwd / util::package_config);
                int lines_modified = cpm_pack.remove(package);
                spdlog::info("cpm_pack.json: modified {} line/s\n", lines_modified);
            }
        }
    }
}

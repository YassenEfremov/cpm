#include "remove.hpp"

#include "command.hpp"
#include "../db/package_db.hpp"
#include "../package.hpp"
#include "../script/cpm_pack.hpp"
#include "../util.hpp"

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdint>

namespace fs = std::filesystem;


namespace cpm {

    RemoveCommand::RemoveCommand(const std::string &name) : Command(name) {}

    void RemoveCommand::run(const std::vector<Package> &packages) {

        for (const auto &package : packages) {

            // Check if the target directory exists
            if (!fs::exists(fs::current_path() / util::packages_dir / package.get_name() / "")) {            // TODO: check the local package DB instead
                throw std::invalid_argument("Package not installed!");
            }

            spdlog::info(
                "Removing package from {} ...\n",
                (fs::current_path() / util::packages_dir / package.get_name() / "").string()
            );

            std::uintmax_t n = fs::remove_all(fs::current_path() / util::packages_dir / package.get_name() / "");
            spdlog::info("Removed {} entries\n", n);

            CPMPack cpm_pack(fs::current_path() / util::package_config);
            int lines_modified = cpm_pack.remove(package);
            spdlog::info("cpm_pack.json: modified {} line/s\n", lines_modified);

            // std::ofstream config_out(util::package_config);
            // config_out << std::setw(4) << config_json;

            // PackageDB db(fs::current_path() / util::packages_dir / util::packages_db);
            // int rows_modified = db.remove(package);
            // spdlog::info("Package DB: modified {} rows\n", rows_modified);
        }
    }
}

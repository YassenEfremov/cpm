#include "remove.hpp"

#include "command.hpp"
#include "../db/db.hpp"
#include "../package.hpp"
#include "../util.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdint>

namespace fs = std::filesystem;


namespace cpm {

    RemoveCommand::RemoveCommand(const std::string &name) : Command(name) {}

    void RemoveCommand::run(const std::vector<Package> &args) {

        for (auto package : args) {

            fs::path package_dir = fs::path(package.get_name()).stem();

            spdlog::info(
                "Removing package from {} ...\n",
                (fs::current_path() / util::packages_dir / package_dir / "").string()
            );

            // Check if the target directory exists
            if (!fs::exists(fs::current_path() / util::packages_dir / package_dir / "")) {            // TODO: check the local package DB instead
                throw std::invalid_argument("Package not installed!");
            }

            std::uintmax_t n = fs::remove_all(fs::current_path() / util::packages_dir / package_dir / "");
            spdlog::info("Removed {} entries\n", n);

            // Remove the package from the local DB
            PackageDB db(fs::current_path() / util::packages_dir / util::packages_db);
            int rows_modified = db.remove(Package(package_dir.string()));

            spdlog::info("DB: modified {} rows\n", rows_modified);
        }
    }
}

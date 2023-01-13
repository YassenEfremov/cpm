#include "remove.hpp"

#include "command.hpp"
#include "../package.hpp"

#include "spdlog/spdlog.h"
#include "sqlite3.h"

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

            spdlog::get("stdout_logger")->info(
                "Removing package from {} ...",
                (fs::current_path() / "lib" / package_dir / "").string()
            );

            // Check if the target directory exists
            if (!fs::exists(fs::current_path() / "lib" / package_dir / "")) {            // TODO: check the local package DB instead
                spdlog::get("stdout_logger")->info("\n");
                throw std::invalid_argument("Package not installed!");
            }

            std::uintmax_t n = fs::remove_all(fs::current_path() / "lib" / package_dir / "");

            spdlog::get("stdout_logger")->info(" done.\n");

            spdlog::get("stdout_logger")->info("Removed {} entries\n", n);

            // Remove the package from the local DB
            sqlite3 *db;
            sqlite3_open("lib/packages.db3", &db);

            sqlite3_stmt *stmt;
            sqlite3_prepare(db,
                "DELETE FROM installed_packages WHERE name = ?;"
            , -1, &stmt, nullptr);
            
            sqlite3_bind_text(stmt, 1, package_dir.string().c_str(), package_dir.string().length(), SQLITE_STATIC);
            sqlite3_step(stmt);
            int rows_modified = sqlite3_total_changes(db);

            sqlite3_finalize(stmt);
            sqlite3_close(db);

            spdlog::get("stdout_logger")->info(
                "DB: modified {} rows\n", rows_modified
            );
        }
    }
}

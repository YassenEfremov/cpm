#include "remove.hpp"

#include "spdlog/spdlog.h"
#include "sqlite3.h"

#include <string>
#include <stdexcept>
#include <filesystem>

#include <cstdint>

namespace fs = std::filesystem;


namespace cpm {

    void remove(const std::string &package) {

        fs::path repository = fs::path(package).stem();

        spdlog::get("stdout_logger")->info(
            "Removing package from {} ...",
            (fs::current_path() / "lib" / repository / "").string()
        );

        // Check if the target directory exists
        if (!fs::exists(fs::current_path() / "lib" / repository / "")) {            // TODO: check the local package DB instead
            spdlog::get("stdout_logger")->info("\n");
            throw std::invalid_argument("Package not installed!");
        }

        std::uintmax_t n = fs::remove_all(fs::current_path() / "lib" / repository / "");

        spdlog::get("stdout_logger")->info(" done.\n");

        spdlog::get("stdout_logger")->info("Removed {} entries\n", n);

        // Remove the package from the local DB
        sqlite3 *db;
        sqlite3_open("lib/packages.db3", &db);

        sqlite3_stmt *stmt;
        sqlite3_prepare(db,
            "DELETE FROM installed_packages WHERE name = ?;"
        , -1, &stmt, nullptr);
        
        sqlite3_bind_text(stmt, 1, repository.string().c_str(), repository.string().length(), SQLITE_STATIC);
        sqlite3_step(stmt);
        int rows_modified = sqlite3_total_changes(db);

        sqlite3_finalize(stmt);
        sqlite3_close(db);

        spdlog::get("stdout_logger")->info(
            "DB: modified {} rows\n", rows_modified
        );
    }
}

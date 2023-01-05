#include "remove.hpp"

#include "sqlite3.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <filesystem>

#include <cstdint>

namespace fs = std::filesystem;


namespace cpm {

    void remove(const std::string &package) {

        fs::path repository = fs::path(package).stem();

        std::cout << "Removing package from "
            << fs::current_path() / "lib" / repository / "" << " ..."
        << std::flush;

        // Check if the target directory exists
        if (!fs::exists(fs::current_path() / "lib" / repository / "")) {
            std::cout << std::endl;
            throw std::invalid_argument("Package not installed!");
        }

        std::uintmax_t n = fs::remove_all(fs::current_path() / "lib" / repository / "");

        std::cout << " done.\n";

        std::cout << "Removed " << n << " entries" << std::endl;

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

        std::cout << "DB: modified " << rows_modified << " rows" << std::endl;
    }
}

#include "remove.hpp"

#include "SQLiteCpp/SQLiteCpp.h"

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
        SQLite::Database db("packages.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        SQLite::Statement query(db,
            "DELETE FROM installed_packages WHERE name = ?;"
        );
        
        query.bind(1, repository);
        int rows_modified = query.exec();

        std::cout << "DB: modified " << rows_modified << " rows" << std::endl;
    }
}

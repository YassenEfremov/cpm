#include "remove.hpp"

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
    }
}

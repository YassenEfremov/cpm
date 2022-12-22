#include "remove.hpp"

#include <iostream>
#include <string>
#include <filesystem>

#include <cstdlib>
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
            std::cout << "\nPackage not installed!" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::uintmax_t n = fs::remove_all(fs::current_path() / "lib" / repository / "");

        std::cout << " done.\n";

        std::cout << "Removed " << n << " entries" << std::endl;
    }
}

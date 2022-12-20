#include "install.hpp"

#include "cpr/cpr.h"

extern "C" {
    #include "zip.h"
}

#include <iostream>
#include <string>
#include <filesystem>

#include <cstdlib>

namespace fs = std::filesystem;


namespace cpm {

    void install(const std::string &package) {

        fs::path repository = fs::path(package).stem();

        // Check if the target directory already exists
        if (fs::exists(fs::current_path() / "lib" / repository / "")) {
            std::cout << "\nPackage directory already exists!" << std::endl;
            std::exit(EXIT_SUCCESS);
        }

        std::cout << "Installing package into "
            << fs::current_path() / "lib" / repository / "" << " ..."
        << std::flush;

        // Download the repository as a zip archive (same as the green button on GitHub)
        cpr::Response response = cpr::Get(cpr::Url{package + "/archive/refs/heads/master.zip"});    // TODO: this should use the github API

        std::cout << " done.\n";

        // Extract the entries from the archive into <cwd>/lib/<repo-name>-master
        struct zip_t *zip = zip_stream_open(response.text.c_str(), response.text.size(), 0, 'r');
        zip_stream_extract(response.text.c_str(), response.text.size(),
            (fs::current_path() / "lib" / "").c_str(),
            [](const char *filename, void *arg) {
                struct zip_t *zip = static_cast<struct zip_t *>(arg);
                static int i = 1;
                std::cout << "\rExtracting archive entries " << i++ << "/" << zip_entries_total(zip);
                return EXIT_SUCCESS;
            }, zip
        );
        std::cout << " done.\n";

        try {
            // Rename the target directory to remove the "-master" at the end
            fs::rename(fs::current_path() / "lib" / repository += "-master/",       // TODO: the in-memory archive name should be changed instead
                       fs::current_path() / "lib" / repository / "");

        } catch (fs::filesystem_error &e) {
            std::cerr << e.what() << "\n";
            std::cerr << "Aborting installation!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

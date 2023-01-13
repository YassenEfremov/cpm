#include "install.hpp"

#include "command.hpp"
#include "../db/db.hpp"
#include "../package.hpp"
#include "../util.hpp"

#include "cpr/cpr.h"
#include "spdlog/spdlog.h"

extern "C" {
    #include "zip.h"
}

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdlib>

namespace fs = std::filesystem;


namespace cpm {

	cpr::Response InstallCommand::download(const std::string &repo_url, void(*progress)()) {
        // Download the repository as a zip archive (same as the green button on GitHub)
        progress();
        return cpr::Get(cpr::Url{repo_url + "/archive/refs/heads/master.zip"});     // TODO: this should use the github API
	}

	void InstallCommand::extract(const std::string &stream,
                                 int(*on_extract)(const char *filename, void *archive)) {
        // Extract the entries from the archive into <cwd>/lib/<repo-name>-master
        struct zip_t *zip = zip_stream_open(stream.c_str(), stream.size(), 0, 'r');

        zip_stream_extract(stream.c_str(), stream.size(),
            (fs::current_path() / util::packages_dir / "").string().c_str(),
            on_extract, zip
        );
	}

	InstallCommand::InstallCommand(const std::string &name) : Command(name) {}

	void InstallCommand::run(const std::vector<Package> &args) {

        for (auto package : args) {

            fs::path package_dir = fs::path(package.get_name()).stem();

            spdlog::info(
                "Installing package into {} ...\n",
                (fs::current_path() / util::packages_dir / package_dir / "").string()
            );

            // Check if the target directory already exists
            if (fs::exists(fs::current_path() / util::packages_dir / package_dir / "")) {             // TODO: check the local package DB instead
                throw std::invalid_argument("Package directory already exists!");
            }

            cpr::Response response = download(package.get_name(), []() {
                spdlog::info("Downloading repository ...");
            });
            spdlog::info(" done.\n");

            extract(response.text, [](const char *filename, void *archive) {
                struct zip_t *zip = static_cast<struct zip_t *>(archive);
                static int n = 1;
                spdlog::info("\rExtracting archive entries {}/{}", n++, zip_entries_total(zip));
                return EXIT_SUCCESS;
            });
            spdlog::info(" done.\n");

            // Rename the target directory to remove the "-master" at the end
            fs::rename(fs::current_path() / util::packages_dir / package_dir += "-master/",           // TODO: the in-memory archive name should be changed instead
                       fs::current_path() / util::packages_dir / package_dir / "");

            // Add the installed package to the local DB
            PackageDB db(fs::current_path() / util::packages_dir / util::packages_db);
            int rows_modified = db.add(Package(package_dir.string()));

            spdlog::info("DB: modified {} rows\n", rows_modified);
        }
    }
}

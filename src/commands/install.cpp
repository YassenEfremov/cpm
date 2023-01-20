#include "install.hpp"

#include "command.hpp"
#include "../db/package_db.hpp"
#include "../package.hpp"
#include "../script/cpm_pack.hpp"
#include "../util.hpp"

#include "cpr/cpr.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

extern "C" {
    #include "zip.h"
}

#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdlib>

namespace fs = std::filesystem;


namespace cpm {

	cpr::Response InstallCommand::download(
        const fs::path &url,
        std::function<bool(
            cpr::cpr_off_t downloadTotal, cpr::cpr_off_t downloadNow,
            cpr::cpr_off_t uploadTotal, cpr::cpr_off_t uploadNow,
            std::intptr_t userdata
        )> progress
    ) {
        // Download the repository as a zip archive (same as the green button on GitHub)
        return cpr::Get(cpr::Url{(url / "archive/refs/heads/master.zip").string()},
                        cpr::ProgressCallback(progress));                           // TODO: this should use the github API
	}

	void InstallCommand::extract(const std::string &stream,
                                 int(*on_extract)(const char *filename, void *archive)) {
        struct zip_t *zip = zip_stream_open(stream.c_str(), stream.size(), 0, 'r');

        zip_stream_extract(stream.c_str(), stream.size(),
            (fs::current_path() / util::packages_dir / "").string().c_str(),
            on_extract, zip
        );
        zip_stream_close(zip);
	}

	InstallCommand::InstallCommand(const std::string &name) : Command(name) {}

	void InstallCommand::run(const std::vector<Package> &packages) {

        for (const auto &package : packages) {

            if (fs::exists(fs::current_path() / util::packages_dir / package.get_name() / "")) {             // TODO: check the local package DB instead
                throw std::invalid_argument("Package directory already exists!");
            }

            spdlog::info(
                "Installing package into {} ...\n",
                (fs::current_path() / util::packages_dir / package.get_name() / "").string()
            );

            cpr::Response response = download(package.get_url(),
                [](cpr::cpr_off_t downloadTotal, cpr::cpr_off_t downloadNow,
                   cpr::cpr_off_t uploadTotal, cpr::cpr_off_t uploadNow, std::intptr_t userdata) {
                    double downloaded = static_cast<double>(downloadNow);
                    double total = static_cast<double>(downloadTotal);
                    std::string unit = "B";
                    if (downloadNow >= 1000000) {
                        downloaded /= 1000000;
                        total /= 1000000;
                        unit = "MB";
                        spdlog::info("\r                                                        ");
                    } else if (downloadNow >= 1000) {
                        downloaded /= 1000;
                        total /= 1000;
                        unit = "KB";
                        spdlog::info("\r                                                        ");
                    }
                    spdlog::info("\rDownloading repository {0:.2f}/{1:.2f} {2}", downloaded, total, unit);
                    return true;
                }
            );
            spdlog::info(" done.\n");

            extract(response.text,
                [](const char *filename, void *archive) {
                    struct zip_t *zip = static_cast<struct zip_t *>(archive);
                    static int n = 1;
                    spdlog::info("\rExtracting archive entries {}/{}", n++, zip_entries_total(zip));
                    return EXIT_SUCCESS;
                }
            );
            spdlog::info(" done.\n");

            // Rename the target directory to remove the "-master" at the end
            fs::rename(fs::current_path() / util::packages_dir / package.get_name() += "-master/",           // TODO: the in-memory archive name should be changed instead
                       fs::current_path() / util::packages_dir / package.get_name() / "");

            CPMPack cpm_pack(fs::current_path() / util::package_config);
            int lines_modified = cpm_pack.add(package);
            spdlog::info("cpm_pack.json: modified {} line/s\n", lines_modified);

            // PackageDB db(fs::current_path() / util::packages_dir / util::packages_db);
            // int rows_modified = db.add(package);
            // spdlog::info("Package DB: modified {} rows\n", rows_modified);
        }
    }
}

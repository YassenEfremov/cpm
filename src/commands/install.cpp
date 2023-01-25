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
#include <functional>
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
        // Download the repository as a zip archive (same as the green button on GitHub)    // TODO: this should use the github API
        return cpr::Get(cpr::Url{(url / "archive/refs/heads/master.zip").string()},
                        cpr::ProgressCallback(progress));
	}

	void InstallCommand::extract(const std::string &stream, const fs::path &output_dir,
                                 bool(*on_extract)(int currentEntry, int totalEntries)) {
        struct zip_t *zip = zip_stream_open(stream.c_str(), stream.size(), 0, 'r');

        std::size_t n = zip_entries_total(zip);
        for (int i = 0; i < n; i++) {
            zip_entry_openbyindex(zip, i);

            std::string entry_name = zip_entry_name(zip);
            std::size_t first_slash = entry_name.find_first_of('/');
            if (zip_entry_isdir(zip)) {
                fs::create_directories(output_dir / entry_name.substr(first_slash + 1));
            }
            zip_entry_fread(zip, (output_dir / entry_name.substr(first_slash + 1)).string().c_str());
            on_extract(i, n);

            zip_entry_close(zip);
        }

        zip_stream_close(zip);
	}

	InstallCommand::InstallCommand(const std::string &name) : Command(name) {}

	void InstallCommand::run() {

        auto package_names = this->get<std::vector<std::string>>("packages");
        auto packages = std::vector<Package>(package_names.begin(), package_names.end());

        fs::path cwd;
        if (this->is_used("--global")) {
            cwd = util::global_dir;
        } else {
            cwd = fs::current_path();
        }

        for (const auto &package : packages) {

            if (fs::exists(cwd / util::packages_dir / package.get_name() / "")) {             // TODO: check the local package DB instead
                throw std::invalid_argument("Package directory already exists!");
            }

            spdlog::info(
                "Installing package into {} ...\n",
                (cwd / util::packages_dir / package.get_name() / "").string()
            );

            cpr::Response response = this->download(package.get_url(),
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

            this->extract(response.text, cwd / util::packages_dir / package.get_name(),
                [](int currentEntry, int totalEntries) {
                    spdlog::info("\rExtracting archive entries {}/{}", currentEntry, totalEntries);
                    return true;
                }
            );
            spdlog::info(" done.\n");

            if (this->is_used("--global")) {
                PackageDB db(cwd / util::package_db);
                int rows_modified = db.add(package);
                spdlog::info("Package DB: modified {} row/s\n", rows_modified);

            } else {
                CPMPack cpm_pack(cwd / util::package_config);
                int lines_modified = cpm_pack.add(package);
                spdlog::info("cpm_pack.json: modified {} line/s\n", lines_modified);
            }
        }
    }
}

#include "commands/install.hpp"

#include "commands/command.hpp"
#include "db/package_db.hpp"
#include "package.hpp"
#include "script/cpm_pack.hpp"
#include "paths.hpp"

#include "cpr/cpr.h"
#include "spdlog/spdlog.h"

extern "C" {
    #include "zip.h"
}

#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdint>
#include <cstdlib>

namespace fs = std::filesystem;


namespace cpm {

	InstallCommand::InstallCommand(const std::string &name) : Command(name) {}

	void InstallCommand::run() {

        auto package_names = this->get<std::vector<std::string>>("packages");
        // auto packages = std::vector<Package>(package_names.begin(), package_names.end());
        std::vector<Package> packages;
        for (const auto &name : package_names) {
            packages.push_back(Package{name});
        }

        for (const auto &package : packages) {

            if (fs::exists(this->context.cwd / paths::packages_dir / package.name / "")) {             // TODO: check the local package database instead
                throw std::invalid_argument("Package directory already exists!");
            }

            spdlog::info(
                "Installing package into {} ...\n",
                (this->context.cwd / paths::packages_dir / package.name / "").string()
            );

            cpr::Response response = this->download(package,
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
                    if (total < downloaded) {
                        total = downloaded;
                    }
                    spdlog::info("\rDownloading repository {0:.2f}/{1:.2f} {2}", downloaded, total, unit);
                    return true;
                }
            );
            spdlog::info(" done.\n");

            this->extract(
                response.text,
                this->context.cwd / paths::packages_dir / package.name,
                [](int currentEntry, int totalEntries) {
                    spdlog::info("\rExtracting archive entries {}/{}", currentEntry, totalEntries);
                    return true;
                }
            );
            spdlog::info(" done.\n");

            int records_modified = this->context.repo->add(package);
            spdlog::info(
                "{}: modified {} record/s\n",
                this->context.repo->get_filename().filename().string(), records_modified
            );
        }
    }

	cpr::Response InstallCommand::download(
        const Package &package,
        std::function<bool(
            cpr::cpr_off_t downloadTotal, cpr::cpr_off_t downloadNow,
            cpr::cpr_off_t uploadTotal, cpr::cpr_off_t uploadNow,
            std::intptr_t userdata
        )> download_progress
    ) {
        cpr::Response res = cpr::Head(
            cpr::Url{(paths::api_url / paths::repo_name / package.name / "zipball").string()}
        );

        if (res.status_code != cpr::status::HTTP_OK) {
            throw std::invalid_argument("Package not found!");
        }

        res = cpr::Get(
            cpr::Url{(paths::api_url / paths::repo_name / package.name / "zipball").string()},
            cpr::ProgressCallback(download_progress)
        );

        return res;
	}

	void InstallCommand::extract(
        const std::string &stream, const fs::path &output_dir,
        std::function<bool(int currentEntry, int totalEntries)> on_extract
    ) {
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
            on_extract(i + 1, n);

            zip_entry_close(zip);
        }

        zip_stream_close(zip);
	}
}
